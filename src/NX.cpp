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
#include "NX.hpp"
#include "lz4.hpp"
#include <cstring>
#ifdef _WIN32
#  define NL_WINDOWS
#ifndef NOMINMAX
#  define NOMINMAX
#endif
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
#pragma region Structs
#pragma pack(push, 1)
    struct File::Header {
        uint32_t const magic;
        uint32_t const ncount;
        uint64_t const noffset;
        uint32_t const scount;
        uint64_t const soffset;
        uint32_t const bcount;
        uint64_t const boffset;
        uint32_t const acount;
        uint64_t const aoffset;
    };
    struct Node::Data {
        uint32_t const name;
        uint32_t const children;
        uint16_t const num;
        Type const type;
        union {
            int64_t const ireal;
            double const dreal;
            uint32_t const string;
            int32_t const vector[2];
            struct {
                uint32_t const bitmap;
                uint16_t const width;
                uint16_t const height;
            };
            struct {
                uint32_t const audio;
                uint32_t const length;
            };
        };
    };
#pragma pack(pop)
#pragma endregion
#pragma region String
    uint16_t String::Size() const {
        if (!d) return 0;
        return *reinterpret_cast<uint16_t const *>(d);
    }
    char const * String::Data() const {
        if (!d) return 0;
        return reinterpret_cast<char const *>(d) + 2;
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
        String s = {nullptr};
        return s;
    }
    String String::Construct(void const * d) {
        String s = {d};
        return s;
    }
    String String::Construct(uint32_t i, File const * f) {
        String s = {reinterpret_cast<char const *>(f->base) + f->stable[i]};
        return s;
    }
#pragma endregion
#pragma region Bitmap
    uint8_t * Bitmap::buf = nullptr;
    size_t Bitmap::len = 1;
    Bitmap Bitmap::Construct(size_t w, size_t h, void const * d) {
        Bitmap b = {w, h, d};
        return b;
    }
    void const * Bitmap::Data() const {
        if (!d) return nullptr;
        size_t l = Length() + 0x10;
        if (len < l) {
            while (len < l) len <<= 1;
            delete buf;
            buf = new uint8_t[len];
        }
        LZ4::Uncompress(reinterpret_cast<uint8_t const *>(d) + 4, buf, Length());
        return buf;
    }
    size_t Bitmap::Width() const {
        return w;
    }
    size_t Bitmap::Height() const {
        return h;
    }
    size_t Bitmap::Length() const {
        return w * h * 4;
    }
#pragma endregion
#pragma region Node
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
        ++d;
        return *this;
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
    Node Node::operator[](char const * o) const {
        return Construct(Get(o, strlen(o)), f);
    }
    Node::Data const * Node::Get(char const * o, size_t l) const {
        if (!d) return nullptr;
        Data const * p = f->ntable + d->children;
        size_t n = d->num;
        if (!n) return nullptr;
        char const * const b = reinterpret_cast<const char *>(f->base);
        uint64_t const * const t = f->stable;
    bloop:
        size_t const n2 = n >> 1;
        Data const * const p2 = p + n2;
        char const * const sl = b + t[p2->name];
        size_t const l1 = *reinterpret_cast<uint16_t const *>(sl);
        uint8_t const * s = reinterpret_cast<uint8_t const *>(sl + 2);
        uint8_t const * os = reinterpret_cast<uint8_t const *>(o);
        for (size_t i = l1 < l ? l1 : l; i; --i, ++s, ++os) {
            if (*s > *os) goto greater;
            if (*s < *os) goto lesser;
        }
        if (l1 < l) goto lesser;
        if (l1 > l) goto greater;
        return p2;
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
        if (d->type == ireal) return d->ireal;
        if (d->type == dreal) return static_cast<int64_t>(d->dreal);
        if (d->type == string) return std::stoll((std::string)String::Construct(d->string, f));
        return 0;
    }
    Node::operator double() const {
        if (!d) return 0;
        if (d->type == dreal) return d->dreal;
        if (d->type == ireal) return static_cast<double>(d->ireal);
        if (d->type == string) return std::stod((std::string)String::Construct(d->string, f));
        return 0;
    }
    Node::operator String() const {
        if (!d) return String::Blank();
        if (d->type == string) return String::Construct(d->string, f);
        return String::Blank();
    }
    Node::operator std::string() const {
        if (!d) return std::string();
        if (d->type == string) return (std::string)String::Construct(d->string, f);
        if (d->type == ireal) return std::to_string(d->ireal);
        if (d->type == dreal) return std::to_string(d->dreal);
        return std::string();
    }
    Node::operator std::pair<int32_t, int32_t>() const {
        if (!d) return std::pair<int32_t, int32_t>(0, 0);
        if (d->type == vector) return std::pair<int32_t, int32_t>(d->vector[0], d->vector[1]);
        return std::pair<int32_t, int32_t>(0, 0);
    }
    Node::operator Bitmap() const {
        if (!d) return Bitmap::Construct(0, 0, nullptr);
        if (d->type == bitmap) return Bitmap::Construct(d->width, d->height, reinterpret_cast<char const *>(f->base) + f->btable[d->bitmap]);
        return Bitmap::Construct(0, 0, nullptr);
    }
    int32_t Node::X() const {
        if (!d) return 0;
        if (d->type == vector) return d->vector[0];
        return 0;
    }
    int32_t Node::Y() const {
        if (!d) return 0;
        if (d->type == vector) return d->vector[1];
        return 0;
    }
    String Node::Name() const {
        if (!d) return String::Blank();
        return String::Construct(d->name, f);
    }
    size_t Node::Num() const {
        if (!d) return 0;
        return d->num;
    }
    Node::Type Node::T() const {
        if (!d) return Type::none;
        return d->type;
    }
    Node Node::Construct(Data const * d, File const * f) {
        Node n = {d, f};
        return n;
    }
#pragma endregion
#pragma region File
    File::File(char const * name) {
#ifdef NL_WINDOWS
        file = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
        if (file == INVALID_HANDLE_VALUE) throw;
        map = CreateFileMappingA(file, 0, PAGE_READONLY, 0, 0, 0);
        if (!map) throw;
        base = MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);
        if (!base) throw;
#elif defined NL_LINUX
        file = open(name, O_RDONLY);
        if (file == -1) throw;
        struct stat finfo;
        if (fstat(file, &finfo) == -1) throw;
        size = finfo.st_size;
        base = mmap(nullptr, size, PROT_READ, MAP_SHARED, file, 0);
        if (reinterpret_cast<size_t>(base) == -1) throw;
#endif
        head = reinterpret_cast<Header const *>(base);
        if (head->magic != 0x34474B50) throw;
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
#elif defined NL_LINUX
        munmap(const_cast<void *>(base), size);
        close(file);
#endif
    }
    Node File::Base() const {
        return Node::Construct(ntable, this);
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
#pragma endregion
}
