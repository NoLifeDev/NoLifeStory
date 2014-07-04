///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////
#pragma once

class MapFile {
public:
    void Open(string filename);
    uint64_t Tell();
    void *TellPtr();
    void Seek(uint64_t);
    void Skip(uint64_t);
    void *ReadBin(uint64_t size);
    template <typename T>
    T Read() {
        return *reinterpret_cast<T *>(ReadBin(sizeof(T)));
    }
    int32_t ReadCInt();
    char *ReadEncString();
    char *ReadPropString(uint32_t offset);
    uint32_t ReadOffset(uint32_t fileStart);

private:
    char *base;
    char *off;
};
char *ToString(int);
string ToString(char *);
char *AllocString(uint16_t len);