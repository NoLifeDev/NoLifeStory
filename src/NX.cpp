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
#include "NX.h"
#include "lz4.h"
#include <cstring>
#include <iostream>
#ifdef _WIN32
#define NL_WINDOWS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
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
#pragma pack(1)
    struct File::Header {
        uint32_t magic;
        uint32_t ncount;
        uint64_t noffset;
        uint32_t strcount;
        uint64_t stroffset;
        uint32_t sprcount;
        uint64_t sproffset;
        uint32_t sndcount;
        uint64_t sndoffset;
    };
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
        return *reinterpret_cast<uint16_t *>(d);
    }
    const char * String::Data() const {
        if (!d) return 0;
        return reinterpret_cast<const char *>(d) + 2;
    }
    String::operator std::string() const {
        if (!d) return std::string();
        return std::string(Data(), Size());
    }
    Node Node::begin() const {
        if (!d) return Construct(nullptr, f);
        return Construct(f->ntable + d->children, f);
    }
    Node Node::end() const {
        if (!d) return Construct(nullptr, f);
        return Construct(f->ntable + d->children + d->num, f);
    }
    Node Node::operator*() const {
        return *this;
    }
    Node Node::operator++() {
        return Construct(++d, f);
    }
    Node Node::operator++(int) {
        return Construct(d++, f);
    }
    bool Node::operator==(Node o) const {
        return d == o.d;
    }
    bool Node::operator!=(Node o) const {
        return d != o.d;
    }
    Node Node::operator[](std::string o) const {
        return Get(o.c_str(), o.length());
    }
    Node Node::operator[](String o) const {
        return Get(o.Data(), o.Size());
    }
    Node Node::operator[](char * o) const {
        return Get(o, strlen(o));
    }
    Node Node::operator[](const char * o) const {
        return Get(o, strlen(o));
    }
    Node Node::Get(const char * o, size_t l) const {
        if (!d) return Construct(nullptr, f);
        Data * p = f->ntable + d->children;
        size_t n = d->num;
        if (!n) return Construct(nullptr, f);
        do {
            size_t n2 = n >> 1;
            Data * p2 = p + n2;
            char * s = reinterpret_cast<char *>(f->base) + f->stable[p2->name];
            size_t l1 = *reinterpret_cast<uint16_t *>(s);
            if (l1 < l) {
                int r = memcmp(s + 2, o, l1);
                if (r > 0) goto greater;
                else goto lesser;
            } else if (l1 > l) {
                int r = memcmp(s + 2, o, l);
                if (r < 0) goto lesser;
                else goto greater;
            } else {
                int r = memcmp(s + 2, o, l);
                if (r < 0) goto lesser;
                else if (r > 0) goto greater;
                else return Construct(p2, f);
            }
        lesser:
            p = p2 + 1;
            n -= n2 + 1;
            continue;
        greater:
            n = n2;
            continue;
        } while (n);
        char * s = reinterpret_cast<char *>(f->base) + f->stable[p->name];
        if (*reinterpret_cast<uint16_t *>(s) != l) return Construct(nullptr, f);
        if (!memcmp(o, s + 2, l)) return Construct(p, f);
        return Construct(nullptr, f);
    }
    int32_t Node::X() const {
        if (!d) return 0;
        if (d->type != Type::vector) return 0;
        return d->vector[0];
    }
    String Node::Name() const {
        String s;
        if (!d) s.d = nullptr;
        else s.d = reinterpret_cast<char *>(f->base) + f->stable[d->name];
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
    Node Node::Construct(Data * d, File * f) {
        Node n;
        n.d = d, n.f = f;
        return n;
    }
    File::File(std::string name) {
#ifdef NL_WINDOWS
        file = CreateFileA(name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
        if (file == INVALID_HANDLE_VALUE) throw;
        map = CreateFileMappingA(file, NULL, PAGE_READONLY, 0, 0, NULL);
        if (!map) throw;
        base = MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);
        if (!base) throw;
#elif defined NL_LINUX
        file = open(name.c_str(), O_RDONLY);
        if (file == -1) throw;
        struct stat finfo;
        if (fstat(int(file), &finfo) == -1) throw;
        size = finfo.st_size;
        base = mmap(NULL, size, PROT_READ, MAP_SHARED, file, 0);
        if (reinterpret_cast<size_t>(base) == -1) throw;
#endif
        head = reinterpret_cast<Header*>(base);
        if (head->magic != 0x34474B50) throw;
        ntable = reinterpret_cast<Node::Data *>(reinterpret_cast<char *>(base) + head->noffset);
        stable = reinterpret_cast<uint64_t *>(reinterpret_cast<char *>(base) + head->stroffset);
    }
    File::~File() {
#ifdef NL_WINDOWS
        UnmapViewOfFile(base);
        CloseHandle(map);
        CloseHandle(file);
#elif defined NL_LINUX
        munmap(base, size);
        close(file);
#endif
    }
    Node File::Base() {
        return Node::Construct(ntable, this);
    }
}
