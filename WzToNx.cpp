#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <cassert>
#include <codecvt>
#include <vector>
#include <queue>
using namespace std;
using namespace std::tr2::sys;
#include "Keys.h"

enum class Type : uint16_t {
    none = 0,
    ireal = 1,
    dreal = 2,
    string = 3,
    vector = 4,
    bitmap = 5,
    audio = 6,
    uol = 7
};
struct Node {
    Node() : name(0), children(0), num(0), type(Type::none) {}
    uint32_t name;
    uint32_t children;
    uint16_t num;
    Type type;
    union {
        int64_t ireal;
        double dreal;
        uint32_t string;
        int32_t vector[2];
        struct {
            uint32_t bitmap;
            uint16_t width;
            uint16_t height;
        };
        struct {
            uint32_t audio;
            uint32_t length;
        };
    };
};
vector<Node> Nodes;
queue<uint32_t> Directories;
queue<pair<uint32_t, uint32_t>> Imgs;
char16_t WSBuf[0x8000];
char SBuf[0x10000];
codecvt_utf8<char16_t> Conv;
uint8_t * Keys[3] = {BMSKey, GMSKey, KMSKey};
uint8_t * Key;
uint32_t FileStart;
void * InFileHandle;
void * InMapHandle;
char * InBase;
char * InOffset;
void * OutFileHandle;
void * OutMapHandle;
char * OutBase;
char * OutOffset;

void OpenInput(path p) {
    assert(is_regular_file(p));
    InFileHandle = CreateFileA(p.string().c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    assert(InFileHandle != INVALID_HANDLE_VALUE);
    InMapHandle= CreateFileMappingA(InFileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
    assert(InMapHandle != nullptr);
    InBase = reinterpret_cast<char *>(MapViewOfFile(InMapHandle, FILE_MAP_READ, 0, 0, 0));
    assert(InBase != nullptr);
    InOffset = InBase;
}
void CloseInput() {
    UnmapViewOfFile(InBase);
    CloseHandle(InMapHandle);
    CloseHandle(InFileHandle);
}
void OpenOutput(path p, uint64_t s) {
    assert(is_regular_file(p));
    OutFileHandle = CreateFileA(p.string().c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
    assert(OutFileHandle != INVALID_HANDLE_VALUE);
    OutMapHandle = CreateFileMappingA(OutFileHandle, nullptr, PAGE_READWRITE, s >> 32, s & 0xffffffff, nullptr);
    assert(OutMapHandle != nullptr);
    OutBase = reinterpret_cast<char *>(MapViewOfFile(OutMapHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0));
    assert(OutBase != nullptr);
    OutOffset = OutBase;
}
void CloseOutput() {
    UnmapViewOfFile(OutBase);
    CloseHandle(OutMapHandle);
    CloseHandle(OutFileHandle);
}
uint64_t InTell() {
    return InOffset - InBase;
}
void InSeek(uint64_t n) {
    InOffset = InBase + n;
}
void InSkip(uint64_t n) {
    InOffset += n;
}
uint64_t OutTell() {
    return OutOffset - OutBase;
}
void OutSeek(uint64_t n) {
    OutOffset = OutBase + n;
}
void OutnSkip(uint64_t n) {
    OutOffset += n;
}
template <typename T> T Read() {
    T & v = * reinterpret_cast<T *>(InOffset);
    InOffset += sizeof(T);
    return v;
}
int32_t ReadCInt() {
    int8_t a = Read<int8_t>();
    if (a != -128) return a;
    return Read<int32_t>();
}
uint32_t ReadEncString() {
    int32_t len = Read<int8_t>();
    assert(len != 0);
    if (len > 0) {
        if (len == 127) len = Read<int32_t>();
        assert(len > 0);
        char16_t * ows = reinterpret_cast<char16_t *>(InOffset);
        InSkip(len * 2);
        char16_t * ws = WSBuf;
        char16_t mask = 0xAAAA;
        char16_t * key = reinterpret_cast<char16_t *>(Key);
        for (size_t i = len; i; --i, ++mask, ++ows, ++ws, ++key) {
            *ws = *ows ^ *key ^ mask;
        }
        mbstate_t state;
        const char16_t * fnext;
        char * tnext;
        Conv.out(state, WSBuf, WSBuf + len, fnext, SBuf, SBuf + 0x10000, tnext);
        cout.write(SBuf, tnext - SBuf).put('\n');
    } else {
        if (len == -128) len = Read<int32_t>();
        else len = -len;
        assert(len > 0);
        char * os = InOffset;
        InSkip(len);
        char * s = SBuf;
        char mask = 0xAA;
        char * key = reinterpret_cast<char *>(Key);
        for (size_t i = len; i; --i, ++mask, ++os, ++s, ++key) {
            *s = *os ^ *key ^ mask;
        }
        cout.write(SBuf, len).put('\n');
    }
    return 0;
}
uint32_t ReadPropString(uint32_t offset) {
    uint8_t a = Read<uint8_t>();
    switch (a) {
    case 0x00:
    case 0x73:
        return ReadEncString();
    case 0x01:
    case 0x1B:
        {
            uint32_t o = Read<uint32_t>() + offset;
            uint64_t p = InTell();
            InSeek(o);
            uint32_t s = ReadEncString();
            InSeek(p);
            return s;
        }
    default:
        assert(false);
    }
    return 0;
}
void DeduceKey() {
    int32_t len = Read<int8_t>();
    assert(len < 0);
    if (len == -128) len = Read<int32_t>();
    else len = -len;
    assert(len > 0);
    Key = nullptr;
    for (uint8_t * key : Keys ) {
        char * os = InOffset;
        char mask = 0xAA;
        char * k = reinterpret_cast<char *>(key);
        bool valid = true;
        for (size_t i = len; i; --i && valid, ++mask, ++os, ++k) {
            char c = *os ^ *k ^ mask;
            if (!isalnum(c, locale::classic()) && c != '.') valid = false;
        }
        if (valid) Key = key;
    }
    assert(Key);
    InSkip(len);
}
void Directory(uint32_t node) {
    Node & n = Nodes[node];
    int32_t count = ReadCInt();
    n.num = count;
    n.children = static_cast<uint32_t>(Nodes.size());
    for (int i = count; i; --i) {
        Nodes.emplace_back();
        Node & nn = Nodes.back();
        uint8_t type = Read<uint8_t>();
        switch (type) {
        case 1:
            assert(false);
            break;
        case 2:{
            int32_t s = Read<int32_t>();
            uint64_t p = InTell();
            InSeek(FileStart + s);
            type = Read<uint8_t>();
            nn.name = ReadEncString();
            InSeek(p);
            break;}
        case 3:
        case 4:
            nn.name = ReadEncString();
            break;
        default:
            assert(false);
        }
        uint32_t size = ReadCInt();
        ReadCInt();
        InSkip(4);
        if (type == 3) Directories.push(static_cast<uint32_t>(Nodes.size() - 1));
        if (type == 4) Imgs.push(pair<uint32_t, uint32_t>(static_cast<uint32_t>(Nodes.size() - 1), size));
    }
}
void Img(pair<uint32_t, uint32_t> info) {
    uint64_t p = InTell();
    Node & n = Nodes[info.first];
    InSeek(p + info.second);
}
void Convert(path filename) {
    OpenInput(filename);
    filename.replace_extension(".nx");
    assert(Read<uint32_t>() == *reinterpret_cast<uint32_t *>("PKG1"));
    InSkip(8);
    FileStart = Read<uint32_t>();
    InSeek(FileStart + 2);
    ReadCInt();
    InSkip(1);
    DeduceKey();
    InSeek(FileStart + 2);
    Nodes.emplace_back();
    Directories.emplace(0);
    while (!Directories.empty()) {
        Directory(Directories.front());
        Directories.pop();
    }
    while (!Imgs.empty()) {
        Img(Imgs.front());
        Imgs.pop();
    }
    CloseInput();
    Nodes.clear();
}
int main(int argc, char ** argv) {
    Convert("Skill.wz");
    //Convert("Data.wz");
    for (int i = 1; i < argc; ++i) {
        Convert(argv[i]);
    }
    system("pause");
    return 0;
}