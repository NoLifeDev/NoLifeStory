///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS 1
#include <Windows.h>
#include "wzmain.h"

codecvt_utf8<wchar_t> conv;

char* to_cstring(int n) {
    char* s = new char[0x20];
    sprintf(s, "%i", n);
    return s;
}

static char* adata = nullptr;
static size_t aremain = 0;

char* AllocString(size_t len) {
    if (aremain < len) {
        adata = (char*)malloc(0x10000);
        aremain = 0x10000;
    }
    char* r = adata;
    aremain -= len;
    adata += len;
    return r;
}

struct MapFile::Data {
    Data() :file(0) {}
    HANDLE file;
    HANDLE map;
    uint32_t size;
    uint32_t gran;
};

void MapFile::Open(string filename) {
    d = new Data();
    SYSTEM_INFO sys;
    GetSystemInfo(&sys);
    d->gran = sys.dwAllocationGranularity;
    d->file = CreateFileA(string(filename).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
    if (d->file == INVALID_HANDLE_VALUE) die();
    d->size = GetFileSize(d->file, NULL);
    d->map = CreateFileMappingA(d->file, NULL, PAGE_READONLY, 0, 0, NULL);
    off = 0;
    moff = 0;
    delta = 0;
    data = nullptr;
}

void MapFile::Map(uint32_t base, uint32_t len) {
    if (data) {
        UnmapViewOfFile(data-delta);
    }
    delta = base%d->gran;
    moff = base-delta;
    off = 0;
    data = (char*)MapViewOfFile(d->map, FILE_MAP_READ, 0, moff, min(len+delta, d->size-moff))+delta;
}

void MapFile::Unmap() {
    if (data) {
        UnmapViewOfFile(data-delta);
        data = nullptr;
    }
}

uint32_t MapFile::Tell() {
    return off;
}

void MapFile::Seek(uint32_t o) {
    off = o;
}

void MapFile::Skip(uint32_t o) {
    off += o;
}

void* MapFile::ReadBin(uint32_t size) {
    void* a = &data[off];
    off += size;
    return a;
}

int32_t MapFile::ReadCInt() {
    int8_t a = Read<int8_t>();
    if (a != -128) return a;
    else return Read<int32_t>();
}

char* MapFile::ReadString() {
    int len = 0;
    for (;data[off+len] != '\0'; ++len);
    char* s = AllocString(len+1);
    memcpy(s, data+off, len+1);
    off += len+1;
    return s;
}

char* MapFile::ReadString(int32_t len) {
    char* s = AllocString(len+1);
    memcpy(s, data+off, len);
    s[len] = '\0';
    off += len;
    return s;
}

wchar_t* MapFile::ReadWString(int32_t len) {
    char* s = AllocString(2*len+3);
    if ((intptr_t)s&1) ++s;
    wchar_t* ws = (wchar_t*)s;
    memcpy(s, data+off, 2*len);
    ws[len] = L'\0';
    off += 2*len;
    return ws;
}

char* MapFile::ReadEncString() {
    int8_t slen = Read<int8_t>();
    if (slen == 0) return nullptr;
    else if (slen > 0) {
        int32_t len;
        if (slen == 127) len = Read<int32_t>();
        else len = slen;
        if (len <= 0) return nullptr;
        wchar_t* ws = ReadWString(len);
        for (int i = 0; i < len; ++i) {
            ws[i] ^= WZ::WKey[i];
        }
        char* s = AllocString(len*conv.max_length());
        mbstate_t state;
        const wchar_t* fnext;
        char* tnext;
        conv.out(state, ws, ws+len, fnext, s, s+len*conv.max_length(), tnext);
        *tnext = '\0';
        return s;
    } else {
        int32_t len;
        if (slen == -128) len = Read<int32_t>();
        else len = -slen;
        if (len <= 0) return nullptr;
        char* s = ReadString(len);
        for (int i = 0; i < len; ++i) {
            s[i] ^= WZ::AKey[i];
        }
        return s;
    }
}

char* MapFile::ReadTypeString() {
    uint8_t a = Read<uint8_t>();
    switch (a) {
    case 0x00:
    case 0x73:
        return ReadEncString();
    case 0x01:
    case 0x1B:
        {
            uint32_t o = Read<int32_t>();
            uint32_t p = Tell();
            Seek(o);
            char* s = ReadEncString();
            Seek(p);
            return s;
        }
    default:
        die();
        return nullptr;
    }
}

uint32_t MapFile::ReadOffset(uint32_t fileStart) {
    uint32_t p = off;
    p = (p-fileStart)^0xFFFFFFFF;
    p *= WZ::VersionHash;
    p -= WZ::OffsetKey;
    p = (p<<(p&0x1F))|(p>>(32-p&0x1F));
    uint32_t more = Read<uint32_t>();
    p ^= more;
    p += fileStart*2;
    return p;
}
