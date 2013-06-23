//////////////////////////////////////////////////////////////////////////////
// NoLifeNx - Part of the NoLifeStory project                               //
// Copyright (C) 2013 Peter Atashian                                        //
//                                                                          //
// This program is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU Affero General Public License as           //
// published by the Free Software Foundation, either version 3 of the       //
// License, or (at your option) any later version.                          //
//                                                                          //
// This program is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
// GNU Affero General Public License for more details.                      //
//                                                                          //
// You should have received a copy of the GNU Affero General Public License //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "NX.hpp"
#ifdef NL_WINDOWS
#  include <Windows.h>
#elif defined NL_POSIX
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <sys/fcntl.h>
#  include <sys/mman.h>
#  include <unistd.h>
#endif
namespace NL {
    File::File(char const * name) {
#ifdef NL_WINDOWS
        file = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
        if (file == INVALID_HANDLE_VALUE) throw "Failed to open file " + std::string(name);
        map = CreateFileMappingA(file, 0, PAGE_READONLY, 0, 0, 0);
        if (!map) throw "Failed to create file mapping for " + std::string(name);
        base = MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);
        if (!base) throw "Failed to map view of file " + std::string(name);
#else
        file = open(name, O_RDONLY);
        if (file == -1) throw "Failed to open file " + std::string(name);
        struct stat finfo;
        if (fstat(file, &finfo) == -1) throw "Failed to obtain file information for " + std::string(name);
        size = finfo.st_size;
        base = mmap(nullptr, size, PROT_READ, MAP_SHARED, file, 0);
        if (reinterpret_cast<intptr_t>(base) == -1) throw "Failed to create memory mapping for " + std::string(name);
#endif
        head = reinterpret_cast<Header const *>(base);
        if (head->magic != 0x34474B50) throw std::string(name) + " is not a PKG4 NX file";
        ntable = reinterpret_cast<Node::Data const *>(reinterpret_cast<char const *>(base) + head->noffset);
        stable = reinterpret_cast<uint64_t const *>(reinterpret_cast<char const *>(base) + head->soffset);
        btable = reinterpret_cast<uint64_t const *>(reinterpret_cast<char const *>(base) + head->boffset);
        atable = reinterpret_cast<uint64_t const *>(reinterpret_cast<char const *>(base) + head->aoffset);
    }
    File::~File() {
#ifdef NL_WINDOWS
        UnmapViewOfFile(base);
        CloseHandle(map);
        CloseHandle(file);
#else
        munmap(const_cast<void *>(base), size);
        close(file);
#endif
    }
    Node File::Base() const {
        return Node(ntable, this);
    }
    File::operator Node() const {
        return Node(ntable, this);
    }
    uint32_t File::StringCount() const {
        return head->scount;
    }
    uint32_t File::BitmapCount() const {
        return head->bcount;
    }
    uint32_t File::AudioCount() const {
        return head->acount;
    }
    uint32_t File::NodeCount() const {
        return head->ncount;
    }
    std::string File::GetString(uint32_t i) const {
        char const * const s = reinterpret_cast<char const *>(base) + stable[i];
        return std::string(s + 2, *reinterpret_cast<uint16_t const *>(s));
    }
}