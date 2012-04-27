///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include <Windows.h>
#include "wzmain.h"

struct MapFile::Data {
    Data() :file(0), map(0), off(0), moff(0), data(0) {}
    HANDLE file;
    HANDLE map;
    uint32_t off;
    uint32_t moff;
    uint32_t size;
    uint32_t delta;
    uint32_t gran;
    char* data;
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
    d->off = 0;
    d->moff = 0;
    d->delta = 0;
    d->data = nullptr;
}

void MapFile::Map(uint32_t base, uint32_t len) {
    if (d->data) {
        UnmapViewOfFile(d->data-d->delta);
    }
    d->delta = base%d->gran;
    d->moff = base-d->delta;
    d->off = 0;
    d->data = (char*)MapViewOfFile(d->map, FILE_MAP_READ, 0, d->moff, min(len+d->delta, d->size-d->moff))+d->delta;
}

void MapFile::Unmap() {
    if (d->data) {
        UnmapViewOfFile(d->data-d->delta);
        d->data = nullptr;
    }
}

uint32_t MapFile::Tell() {
    return d->off;
}

void MapFile::Seek(uint32_t o) {
    d->off = o;
}

void MapFile::Skip(uint32_t o) {
    d->off += o;
}

void* MapFile::ReadBin(uint32_t size) {
    void* a = &d->data[d->off];
    d->off += size;
    return a;
}

int32_t MapFile::ReadCInt() {
    int8_t a = Read<int8_t>();
    if (a != -128) return a;
    else return Read<int32_t>();
}

string MapFile::ReadString() {
    string s;
    while(true) {
        char c = d->data[d->off++];
        if (c == '\0') break;
        s += c;
    }
    return s;
}

string MapFile::ReadString(int32_t len) {
    string s = string(&d->data[d->off], len);
    d->off += len;
    return s;
}

wstring MapFile::ReadWString(int32_t len) {
    wstring s = wstring((wchar_t*)&d->data[d->off], len);
    d->off += len*2;
    return s;
}

string MapFile::ReadEncString() {
    int8_t slen = Read<int8_t>();
    if (slen == 0) return string();
    else if (slen > 0) {
        int32_t len;
        if (slen == 127) len = Read<int32_t>();
        else len = slen;
        if (len <= 0) return string();
        uint16_t mask = 0xAAAA;
        wstring ws = ReadWString(len);
        for (int i = 0; i < len; ++i) {
            ws[i] ^= mask;
            ws[i] ^= *(uint16_t*)(WZ::Key+2*i);
            mask++;
        }
        wstring_convert<codecvt_utf8<wchar_t>> conv;
        return conv.to_bytes(ws);
    } else {
        int32_t len;
        if (slen == -128) len = Read<int32_t>();
        else len = -slen;
        if (len <= 0) return string();
        uint8_t mask = 0xAA;
        string s = ReadString(len);
        for (int i = 0; i < len; ++i) {
            s[i] ^= mask;
            s[i] ^= WZ::Key[i];
            mask++;
        }
        return s;
    }
}

string MapFile::ReadTypeString() {
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
            string s = ReadEncString();
            Seek(p);
            return s;
        }
    default:
        die();
        return string();
    }
}

uint32_t MapFile::ReadOffset(uint32_t fileStart) {
    uint32_t p = d->off;
    p = (p-fileStart)^0xFFFFFFFF;
    p *= WZ::VersionHash;
    p -= WZ::OffsetKey;
    p = (p<<(p&0x1F))|(p>>(32-p&0x1F));
    uint32_t more = Read<uint32_t>();
    p ^= more;
    p += fileStart*2;
    return p;
}