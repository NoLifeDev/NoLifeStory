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
#include <cstring>
#include "lz4.h"
#include "NX.h"
#ifdef _WIN32
#define NL_WINDOWS
#include <Windows.h>
#elif defined __linux__
#define NL_LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

namespace NL {
    namespace File {
        char* Base;
#ifdef NL_WINDOWS
        HANDLE file = 0, map;
        void Open(std::string filename) {
            if (file) {
                UnmapViewOfFile(Base);
                CloseHandle(map);
                CloseHandle(file);
            }
            file = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
            if (file == INVALID_HANDLE_VALUE) throw;
            map = CreateFileMappingA(file, NULL, PAGE_READONLY, 0, 0, NULL);
            if (!map) throw;
            Base = reinterpret_cast<char*>(MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0));
            if (!Base) throw;
        }
#elif defined NL_LINUX
        int file = -1;
        off_t fsize = 0;
        void Open(std::string filename) {
            if(file != -1) {
                munmap(Base, fsize);
                close(file);
            }
            file = open(filename.c_str(), O_RDONLY);
            if(file == -1) throw;
            struct stat finfo;
            if(fstat(int(file), &finfo) == -1) throw;
            fsize = finfo.st_size;
            Base = reinterpret_cast<char*>(mmap(NULL, fsize, PROT_READ, MAP_SHARED, file, 0));
            if(reinterpret_cast<size_t>(Base) == -1) throw;
        }
#endif
    }
#pragma pack(1)
    struct Header {
        uint32_t Magic;
        uint32_t NodeCount;
        uint64_t NodeOffset;
        uint32_t StringCount;
        uint64_t StringOffset;
        uint32_t SpriteCount;
        uint64_t SpriteOffset;
        uint32_t SoundCount;
        uint64_t SoundOffset;
    };
    Header* Head;
    uint64_t* StringTable;
    Node::Data* NodeTable;
    Node Base;
#pragma pack(1)
    struct Node::Data {
        uint32_t name;
        uint32_t children;
        uint16_t num;
        Type type;
        union {
            int64_t ireal;
            double dreal;
            uint32_t string;
            int32_t vector[2];
            uint32_t sprite;
            uint32_t sound;
        };
    };
    uint16_t String::Size() const {
        if (!d) return 0;
        return *reinterpret_cast<uint16_t*>(d);
    }
    char* String::Data() const {
        if (!d) return 0;
        return reinterpret_cast<char*>(d)+2;
    }
    String::operator std::string() const {
        if (!d) return std::string();
        return std::string(Data(), Size());
    }
    Node Node::begin() const {
        if (!d) return Node();
        return NodeTable + d->children;
    }
    Node Node::end() const {
        if (!d) return Node();
        return NodeTable + d->children + d->num;
    }
    Node Node::operator*() const {
        return *this;
    }
    Node Node::operator++() {
        return ++d;
    }
    Node Node::operator++(int) {
        return d++;
    }
    bool Node::operator==(Node o) const {
        return d == o.d;
    }
    bool Node::operator!=(Node o) const {
        return d != o.d;
    }
    Node& Node::operator=(Node o) {
        d = o.d;
        return *this;
    }
    Node Node::operator[](std::string o) const {
        return Get(o.c_str(), o.length());
    }
    Node Node::operator[](char* o) const {
        return Get(o, strlen(o));
    }
    Node Node::operator[](const char* o) const {
        return Get(o, strlen(o));
    }
    Node Node::Get(const char* o, size_t l) const {
        if (!d) return Node();
        Data* nd = Base.d + d->children;
        for (size_t i = d->num; i > 0; --i, ++nd) {
            char *s = File::Base + StringTable[nd->name];
            if (*(uint16_t*)s != l) continue;
            if (!memcmp(o, s+2, l)) return nd;
        }
        return Node();
    }
    int32_t Node::X() const {
        if (!d) return 0;
        if (d->type != Type::vector) return 0;
        return d->vector[0];
    }
    String Node::Name() const {
        String s;
        if (!d) s.d = nullptr;
        else s.d = File::Base + StringTable[d->name];
        return s;
    }
    size_t Node::Num() const {
        if (!d) return 0;
        return d->num;
    }
    Node::Type Node::T() const {
        if (!d) return Type::none;
        return d->type;
    }
    void Load(std::string filename) {
        File::Open(filename);
        Head = reinterpret_cast<Header*>(File::Base);
        if (Head->Magic != 0x34474B50) throw;
        NodeTable = reinterpret_cast<Node::Data*>(File::Base + Head->NodeOffset);
        Base.d = NodeTable;
        StringTable = reinterpret_cast<uint64_t*>(File::Base + Head->StringOffset);
    }

    void Recurse(Node::Data* n) {
        size_t i = n->num;
        if (!i) return;
        Node::Data* d = NodeTable + n->children;
        loop:
        Recurse(d);
        ++d;
        if (!!--i) goto loop;
    }
}
