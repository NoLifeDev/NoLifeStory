//////////////////////////////////////////////////////////////////////////
// Copyright 2012 Peter Atechian (Retep998)                             //
//////////////////////////////////////////////////////////////////////////
// This file is part of the NoLifeStory project.                        //
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
    void MapFile::Open(string filename) {
        HANDLE file = CreateFileA(string(filename).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
        if (file == INVALID_HANDLE_VALUE) throw;
        HANDLE map = CreateFileMappingA(file, NULL, PAGE_READONLY, 0, 0, NULL);
        if (!map) throw;
        base = reinterpret_cast<char*>(MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0));
        if (!base) throw;
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
}