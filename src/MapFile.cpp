//////////////////////////////////////////////////////////////////////////
// Copyright 2012 Peter Atechian (Retep998)                             //
//////////////////////////////////////////////////////////////////////////
// This file is part of NoLifeNx.                                       //
//                                                                      //
// NoLifeStory is free software: you can redistribute it and/or modify  //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// NoLifeStory is distributed in the hope that it will be useful,       //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with NoLifeStory.  If not, see <http://www.gnu.org/licenses/>. //
//////////////////////////////////////////////////////////////////////////
#include "Global.h"

namespace NL {
    struct MapFile::Data {
        HANDLE file;
        HANDLE map;
        void* data;
    };
    void MapFile::operator=(MapFile other) {
        d = other.d;
        off = other.off;
    }
    void MapFile::Open(string filename) {
        d = new Data();
        d->file = CreateFileA(string(filename).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
        if (d->file == INVALID_HANDLE_VALUE) die();
        d->map = CreateFileMappingA(d->file, NULL, PAGE_READONLY, 0, 0, NULL);
        if (!d->map) die();
        d->data = MapViewOfFile(d->map, FILE_MAP_READ, 0, 0, 0);
        if (!d->data) die();
        off = 0;
    }
    uint64_t MapFile::Tell() {
        return off;
    }
    void* MapFile::TellPtr() {
        return (char*)d->data + off;
    }
    void MapFile::Seek(uint64_t o) {
        off = o;
    }
    void MapFile::Skip(uint64_t o) {
        off += o;
    }
    void* MapFile::ReadBin(uint64_t size) {
        void* a = (char*)d->data + off;
        off += size;
        return a;
    }
    string MapFile::ReadString(uint16_t length) {
        return string((char*)ReadBin(length), length);
    }
}