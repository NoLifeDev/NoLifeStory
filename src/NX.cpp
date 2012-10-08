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
        const uint32_t magic;
        const uint32_t ncount;
        const uint64_t noffset;
        const uint32_t strcount;
        const uint64_t stroffset;
        const uint32_t sprcount;
        const uint64_t sproffset;
        const uint32_t sndcount;
        const uint64_t sndoffset;
    };
#pragma pack(1)
    struct Node::Data {
        const uint32_t name;
        const uint32_t children;
        const uint16_t num;
        const Type type;
        const union {
            const int64_t ireal;
            const double dreal;
            const uint32_t string;
            const int32_t vector[2];
            const struct {
                const uint32_t sprite;
                const uint16_t width;
                const uint16_t height;
            };
            const struct {
                const uint32_t sound;
                const uint32_t length;
            };
        };
    };
    uint16_t String::Size() const {
        if (!d) return 0;
        return *reinterpret_cast<const uint16_t *>(d);
    }
    const char * String::Data() const {
        if (!d) return 0;
        return reinterpret_cast<const char *>(d) + 2;
    }
    String::operator std::string() const {
        if (!d) return std::string();
        return std::string(Data(), Size());
    }
    bool String::operator==(String s) const {
        return d == s.d;
    }
    bool String::operator!=(String s) const {
        return d != s.d;
    }
    String String::Blank() {
        String s;
        s.d = 0;
        return s;
    }
    String String::Construct(const void * d) {
        String s;
        s.d = d;
        return s;
    }
    String String::Construct(uint32_t i, const File * f) {
        String s;
        s.d = reinterpret_cast<const char *>(f->base) + f->stable[i];
        return s;
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
        return Construct(Get(o.c_str(), o.length()), f);
    }
    Node Node::operator[](String o) const {
        return Construct(Get(o.Data(), o.Size()), f);
    }
    Node Node::operator[](char * o) const {
        return Construct(Get(o, strlen(o)), f);
    }
    Node Node::operator[](const char * o) const {
        return Construct(Get(o, strlen(o)), f);
    }
    const Node::Data * Node::Get(const char * o, size_t l) const {
        if (!d) return nullptr;
        const Data * p = f->ntable + d->children;
        size_t n = d->num;
        if (!n) return nullptr;
        const char * const b = reinterpret_cast<const char *>(f->base);
        const uint64_t * const t = f->stable;
    bloop:
        const size_t n2 = n >> 1;
        const Data * const p2 = p + n2;
        const char * const s = b + t[p2->name];
        const size_t l1 = *reinterpret_cast<const uint16_t *>(s);
        if (l1 < l) {
            const int r = memcmp(s + 2, o, l1);
            if (r > 0) goto greater;
            else goto lesser;
        } else if (l1 > l) {
            const int r = memcmp(s + 2, o, l);
            if (r < 0) goto lesser;
            else goto greater;
        } else {
            const int r = memcmp(s + 2, o, l);
            if (r < 0) goto lesser;
            else if (r > 0) goto greater;
            else return p2;
        }
    lesser:
        p = p2 + 1;
        n -= n2 + 1;
        goto bloop;
    greater:
        n = n2;
        if (!n) return nullptr;
        else goto bloop;
    }
    Node::operator int64_t() const {
        if (!d) return 0;
        else if (d->type == Type::ireal) return d->ireal;
        else if (d->type == Type::dreal) return static_cast<int64_t>(d->dreal);
        else if (d->type == Type::string) return std::stoll((std::string)String::Construct(d->string, f));
        else return 0;
    }
    Node::operator double() const {
        if (!d) return 0;
        else if (d->type == Type::dreal) return d->dreal;
        else if (d->type == Type::ireal) return static_cast<double>(d->ireal);
        else if (d->type == Type::string) return std::stod((std::string)String::Construct(d->string, f));
        else return 0;
    }
    Node::operator String() const {
        if (!d) return String::Blank();
        else if (d->type == Type::string) return String::Construct(d->string, f);
        else return String::Blank();
    }
    Node::operator std::string() const {
        if (!d) return std::string();
        else if (d->type == Type::string) return (std::string)String::Construct(d->string, f);
        else if (d->type == Type::ireal) return std::to_string(d->ireal);
        else if (d->type == Type::dreal) return std::to_string(d->dreal);
        else return std::string();
    }
    Node::operator std::pair<int32_t, int32_t>() const {
        if (!d) return std::pair<int32_t, int32_t>(0, 0);
        else if (d->type == Type::vector) return std::pair<int32_t, int32_t>(d->vector[0], d->vector[1]);
        else return std::pair<int32_t, int32_t>(0, 0);
    }
    int32_t Node::X() const {
        if (!d) return 0;
        else if (d->type != Type::vector) return 0;
        else return d->vector[0];
    }
    int32_t Node::Y() const {
        if (!d) return 0;
        else if (d->type != Type::vector) return 0;
        else return d->vector[1];
    }
    String Node::Name() const {
        if (!d) return String::Blank();
        else return String::Construct(d->name, f);
    }
    size_t Node::Num() const {
        if (!d) return 0;
        else return d->num;
    }
    Node::Type Node::T() const {
        if (!d) return Type::none;
        else return d->type;
    }
    Node Node::Construct(const Data * d, const File * f) {
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
        if (fstat(file, &finfo) == -1) throw;
        size = finfo.st_size;
        base = mmap(nullptr, size, PROT_READ, MAP_SHARED, file, 0);
        if (reinterpret_cast<size_t>(base) == -1) throw;
#endif
        head = reinterpret_cast<const Header *>(base);
        if (head->magic != 0x34474B50) throw;
        ntable = reinterpret_cast<const Node::Data *>(reinterpret_cast<const char *>(base) + head->noffset);
        stable = reinterpret_cast<const uint64_t *>(reinterpret_cast<const char *>(base) + head->stroffset);
    }
    File::~File() {
#ifdef NL_WINDOWS
        UnmapViewOfFile(base);
        CloseHandle(map);
        CloseHandle(file);
#elif defined NL_LINUX
        munmap(const_cast<void *>(base), size);
        close(file);
#endif
    }
    Node File::Base() const {
        return Node::Construct(ntable, this);
    }
}
