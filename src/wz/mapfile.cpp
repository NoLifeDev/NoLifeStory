///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS 1
#include <Windows.h>
#include "wzmain.h"

char* ToString(int n) {
    string str = to_string(n);
    char* s = AllocString(str.length()+1);
    memcpy(s, str.c_str(), str.length()+1);
    return s;
}
char* AllocString(uint16_t len) {
    static char* adata = nullptr;
    static uint32_t aremain = 0;
    if (aremain < len) {
        adata = (char*)malloc(0x100000);
        aremain = 0x100000;
    }
    char* r = adata;
    aremain -= len;
    adata += len;
    return r;
}
void MapFile::Open(string filename) {
    HANDLE file = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
    if (file == INVALID_HANDLE_VALUE) throw("Failed to open file");
    HANDLE map = CreateFileMappingA(file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!map) throw("Failed to create file mapping");
    base = reinterpret_cast<char*>(MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0));
    if (!base) throw("Failed to map view of file");
    off = base;
}
uint64_t MapFile::Tell() {
    return off - base;
}
void* MapFile::TellPtr() {
    return off;
}
void MapFile::Seek(uint64_t o) {
    off = base + o;
}
void MapFile::Skip(uint64_t o) {
    off += o;
}
void* MapFile::ReadBin(uint64_t size) {
    void* a = off;
    off += size;
    return a;
}
int32_t MapFile::ReadCInt() {
    int8_t a = Read<int8_t>();
    if (a != -128) return a;
    else return Read<int32_t>();
}
char* MapFile::ReadEncString() {
    static codecvt_utf8<char16_t> conv;
    static char16_t ws[0x8000];
    static char ns[0x10000];
    int32_t len = Read<int8_t>();
    if (len == 0) return nullptr;
    else if (len > 0) {
        if (len == 127) len = Read<int32_t>();
        if (len <= 0) return nullptr;
        char16_t* ows = reinterpret_cast<char16_t*>(ReadBin(len*2));
        __m128i *m1 = reinterpret_cast<__m128i*>(ws), *m2 = reinterpret_cast<__m128i*>(ows), *m3 = reinterpret_cast<__m128i*>(WZ::WKey);
        for (int i = 0; i <= len>>3; ++i) {
            _mm_store_si128(m1+i, _mm_xor_si128(_mm_loadu_si128(m2+i), _mm_load_si128(m3+i)));
        }
        mbstate_t state;
        const char16_t* fnext;
        char* tnext;
        conv.out(state, ws, ws+len, fnext, ns, ns+0x10000, tnext);
        char* s = AllocString(tnext-ns+1);
        memcpy(s, ns, tnext-ns+1);
        s[tnext-ns] = '\0';
        return s;
    } else {
        if (len == -128) len = Read<int32_t>();
        else len = -len;
        if (len <= 0) return nullptr;
        char* os = reinterpret_cast<char*>(ReadBin(len));
        __m128i *m1 = reinterpret_cast<__m128i*>(ns), *m2 = reinterpret_cast<__m128i*>(os), *m3 = reinterpret_cast<__m128i*>(WZ::AKey);
        for (int i = 0; i <= len>>4; ++i) {
            _mm_store_si128(m1+i, _mm_xor_si128(_mm_loadu_si128(m2+i), _mm_load_si128(m3+i)));
        }
        char* s = AllocString(len+1);
        memcpy(s, ns, len);
        s[len] = '\0';
        return s;
    }
}
char* MapFile::ReadPropString(uint32_t offset) {
    uint8_t a = Read<uint8_t>();
    switch (a) {
    case 0x00:
    case 0x73:
        return ReadEncString();
    case 0x01:
    case 0x1B:
        {
            uint32_t o = Read<uint32_t>()+offset;
            uint64_t p = Tell();
            Seek(o);
            char* s = ReadEncString();
            Seek(p);
            return s;
        }
    default:
        throw;
    }
}
uint32_t MapFile::ReadOffset(uint32_t fileStart) {
    uint32_t p = ~(off-base-fileStart);
    p *= WZ::VersionHash;
    p -= WZ::OffsetKey;
    p = (p<<(p&0x1F))|(p>>(32-p&0x1F));
    uint32_t more = Read<uint32_t>();
    p ^= more;
    p += fileStart*2;
    return p;
}