///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////
#pragma once

class MapFile {
public:
    void Open(string filename);
    void Map(uint32_t base, uint32_t len);
    MapFile() : off(0), moff(0), data(0), delta(0), d(0) {}
    MapFile(const MapFile& other) : off(0), moff(0), data(0), delta(0), d(other.d) {}
    void Unmap();
    uint32_t Tell();
    void Seek(uint32_t);
    void Skip(uint32_t);
    template <class T> T Read() {
        return *(T*)ReadBin(sizeof(T));
    }
    int32_t ReadCInt();
    string ReadEncString();
    string ReadString();
    string ReadString(int32_t len);
    string ReadTypeString();
    wstring ReadWString(int32_t len);
    uint32_t ReadOffset(uint32_t fileStart);
private:
    void* ReadBin(uint32_t size);
    struct Data;
    Data* d;
    uint32_t off;
    uint32_t moff;
    char* data;
    uint32_t delta;
};