#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <codecvt>
#include <vector>
#include <queue>
#include <map>
using namespace std;
using namespace std::tr2::sys;

extern uint8_t BMSKey[65536];
extern uint8_t GMSKey[65536];
extern uint8_t KMSKey[65536];

namespace {
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
    struct String {
        char * data;
        uint16_t size;
    };
    struct StringHash {
        uint32_t hash;
        uint32_t id;
        StringHash * next;
    };
    StringHash * StringSet[0x10000] = {};
    String Strings[0x100000];
    uint32_t NumStrings = 0;
    Node * Nodes;
    uint32_t NumNodes = 1;
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
    char * AllocBuffer = nullptr;
    uint64_t AllocRemain = 0;

    void die(string s) {
        cerr << s << endl;
        throw;
    }
    void * Alloc(uint64_t size) {
        return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    }
    void * SmallAlloc(uint64_t size) {
        if (size > AllocRemain) {
            AllocBuffer = reinterpret_cast<char *>(Alloc(0x1000000));
            AllocRemain = 0x1000000;
        }
        AllocRemain -= size;
        void * v = AllocBuffer;
        AllocBuffer += size;
        return v;
    }
    void OpenInput(path p) {
        if (!is_regular_file(p)) die(p.string() + " is not a valid file");
        InFileHandle = CreateFileA(p.string().c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
        if (InFileHandle == INVALID_HANDLE_VALUE) die("Failed to open input file handle");
        InMapHandle = CreateFileMappingA(InFileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
        if (InMapHandle == nullptr) die("Failed to create a input file mapping");
        InBase = reinterpret_cast<char *>(MapViewOfFile(InMapHandle, FILE_MAP_READ, 0, 0, 0));
        if (InBase == nullptr) die("Failed to map a view of the input file");
        InOffset = InBase;
    }
    void CloseInput() {
        UnmapViewOfFile(InBase);
        CloseHandle(InMapHandle);
        CloseHandle(InFileHandle);
    }
    void OpenOutput(path p, uint64_t s) {
        OutFileHandle = CreateFileA(p.string().c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
        if (OutFileHandle == INVALID_HANDLE_VALUE) die("Failed to open output file handle");
        OutMapHandle = CreateFileMappingA(OutFileHandle, nullptr, PAGE_READWRITE, s >> 32, s & 0xffffffff, nullptr);
        if (OutMapHandle == nullptr) die("Failed to create a output file mapping");
        OutBase = reinterpret_cast<char *>(MapViewOfFile(OutMapHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0));
        if (OutBase == nullptr) die("Failed to map a view of the output file");
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
    uint32_t AddString(char const * data, uint16_t size) {
        uint32_t v = 2166136261UL;
        char const * s = data;
        for (size_t i = size; i; --i, ++s) {
            v ^= static_cast<uint32_t>(*s);
            v *= 16777619UL;
        }
        StringHash * h = StringSet[v & 0xffff];
        if (h) {
            for (;;) {
                if (h->hash == v) return h->id;
                if (h->next) h = h->next;
                else break;
            }
            h->next = reinterpret_cast<StringHash *>(SmallAlloc(sizeof(StringHash)));
            h = h->next;
        } else {
            h = StringSet[v & 0xffff] = reinterpret_cast<StringHash *>(SmallAlloc(sizeof(StringHash)));
        }
        h->hash = v;
        h->id = NumStrings;
        h->next = nullptr;
        Strings[NumStrings].data = reinterpret_cast<char *>(SmallAlloc(size));
        memcpy(Strings[NumStrings].data, data, size);
        Strings[NumStrings].size = size;
        return NumStrings++;
    }
    uint32_t ReadEncString() {
        int32_t len = Read<int8_t>();
        if (len == 0) return 0;
        if (len > 0) {
            if (len == 127) len = Read<int32_t>();
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
            len = static_cast<int32_t>(tnext - SBuf);
        } else {
            if (len == -128) len = Read<int32_t>();
            else len = -len;
            char * os = InOffset;
            InSkip(len);
            char * s = SBuf;
            char mask = 0xAA;
            char * key = reinterpret_cast<char *>(Key);
            for (size_t i = len; i; --i, ++mask, ++os, ++s, ++key) {
                *s = *os ^ *key ^ mask;
            }

        }
        return AddString(SBuf, len);
    }
    uint32_t ReadPropString(uint64_t offset) {
        uint8_t a = Read<uint8_t>();
        switch (a) {
        case 0x00:
        case 0x73:
            return ReadEncString();
        case 0x01:
        case 0x1B:
            {
                uint64_t o = Read<uint32_t>() + offset;
                uint64_t p = InTell();
                InSeek(o);
                uint32_t s = ReadEncString();
                InSeek(p);
                return s;
            }
        default:
            die("Unknown property string type: " + to_string(a));
        }
        return 0;
    }
    void DeduceKey() {
        int32_t len = Read<int8_t>();
        if (len == -128) len = Read<int32_t>();
        else len = -len;
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
        if (!Key) die("Failed to identify the locale");
        InSkip(len);
    }
    void Directory(uint32_t node) {
        Node & n = Nodes[node];
        int32_t count = ReadCInt();
        n.num = count;
        n.children = NumNodes;
        for (int i = 0; i < count; ++i) {
            Node & nn = Nodes[NumNodes];
            uint8_t type = Read<uint8_t>();
            switch (type) {
            case 1:
                die("Found the elusive type 1 directory");
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
                die("Unknown directory type");
            }
            uint32_t size = ReadCInt();
            ReadCInt();
            InSkip(4);
            if (type == 3) Directories.push(NumNodes);
            else if (type == 4) Imgs.push(pair<uint32_t, uint32_t>(NumNodes, size));
            else die("Unknown type 2 directory");
            ++NumNodes;
        }
    }
    void SubProperty(uint32_t node, uint64_t offset);
    void ExtendedProperty(uint32_t node, uint64_t offset) {
        Node & n = Nodes[node];
        uint32_t s = ReadPropString(offset);
        String st = Strings[s];
        if (!strncmp(st.data, "Property", st.size)) {
            InSkip(2);
            SubProperty(node, offset);
        } else if (!strncmp(st.data, "Canvas", st.size)) {
            InSkip(1);
            if (Read<uint8_t>() == 1) {
                InSkip(2);
                SubProperty(node, offset);
            }
            //Canvas stuff
        } else if (!strncmp(st.data, "Shape2D#Vector2D", st.size)) {
            n.type = Type::vector;
            n.vector[0] = ReadCInt();
            n.vector[1] = ReadCInt();
        } else if (!strncmp(st.data, "Shape2D#Convex2D", st.size)) {
            int32_t ec = ReadCInt();
            uint32_t ni = NumNodes;
            NumNodes += ec;
            for (int i = 0; i < ec; ++i, ++ni) {
                Node & nn = Nodes[ni];
                string es = to_string(i);
                nn.name = AddString(es.c_str(), static_cast<uint16_t>(es.size()));
                ExtendedProperty(ni, offset);
            }
        } else if (!strncmp(st.data, "Sound_DX8", st.size)) {
            //Audio stuff
        } else if (!strncmp(st.data, "UOL", st.size)) {
            n.type = Type::uol;
            n.string = ReadPropString(offset);
        } else die("Unknown ExtendedProperty type");
    }
    void SubProperty(uint32_t node, uint64_t offset) {
        int32_t count = ReadCInt();
        uint32_t ni = NumNodes;
        NumNodes += count;
        Node & n = Nodes[node];
        n.num = count;
        n.children = ni;
        for (int i = 0; i < count; ++i, ++ni) {
            Node & nn = Nodes[ni];
            nn.name = ReadPropString(offset);
            uint8_t type = Read<uint8_t>();
            switch (type) {
            case 0x00:
                nn.type = Type::ireal;
                nn.ireal = i;
                break;
            case 0x0B:
            case 0x02:
                nn.type = Type::ireal;
                nn.ireal = Read<uint16_t>();
                break;
            case 0x03:
                nn.type = Type::ireal;
                nn.ireal = ReadCInt();
                break;
            case 0x04:
                nn.type = Type::dreal;
                if (Read<uint8_t>() == 0x80) nn.dreal = Read<float>();
                else nn.dreal = 0.f;
                break;
            case 0x05:
                nn.type = Type::dreal;
                nn.dreal = Read<double>();
                break;
            case 0x08:
                nn.type = Type::string;
                nn.string = ReadPropString(offset);
                break;
            case 0x09:{
                uint64_t p = Read<uint32_t>() + InTell();
                ExtendedProperty(ni, offset);
                InSeek(p);
                break;}
            }
        }
    }
    void Img(uint32_t node, uint32_t size) {
        uint64_t p = InTell();
        InSkip(1);
        DeduceKey();
        InSkip(2);
        SubProperty(node, p);
        InSeek(p + size);
    }
    void Convert(path filename) {
        OpenInput(filename);
        filename.replace_extension(".nx");
        uint32_t magic = Read<uint32_t>();
        if (magic != *reinterpret_cast<uint32_t *>("PKG1")) die("Not a valid WZ file");
        InSkip(8);
        FileStart = Read<uint32_t>();
        InSeek(FileStart + 2);
        ReadCInt();
        InSkip(1);
        DeduceKey();
        InSeek(FileStart + 2);
        Nodes = reinterpret_cast<Node *>(Alloc(0x100000000));
        AddString("", 0);
        Directories.emplace(0);
        while (!Directories.empty()) {
            Directory(Directories.front());
            Directories.pop();
        }
        while (!Imgs.empty()) {
            Img(Imgs.front().first, Imgs.front().second);
            Imgs.pop();
        }
        CloseInput();
    }
}
int main(int argc, char ** argv) {
    Sleep(1000);
    clock_t t1 = clock();
    if (argc > 1) Convert(argv[1]);
    else Convert("Data.wz");
    clock_t t2 = clock();
    cout << "Took " << t2 - t1 << " ms" << endl;
}