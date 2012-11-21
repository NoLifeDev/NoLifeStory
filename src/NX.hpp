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
#include <string>
#include <cstdint>
namespace NL {
    class String;
    class Bitmap;
    class Audio;
    class Node;
    class File;
    class Bitmap {
    public:
        Bitmap() : w(0), h(0), d(nullptr) {}
        Bitmap(uint16_t w, uint16_t h, void const * d) : w(w), h(h), d(d) {}
        Bitmap(Bitmap && o) : w(std::move(o.w)), h(std::move(o.h)), d(std::move(o.d)) {}
        Bitmap(const Bitmap & o) : w(o.w), h(o.h), d(o.d) {}
        void const * Data() const;
        uint16_t Width() const;
        uint16_t Height() const;
        uint32_t Length() const;
    private:
        void const * d;
        uint16_t w, h;
        friend Node;
    };
    class Audio {
    public:
        Audio() : l(0), d(nullptr) {}
        Audio(uint32_t l, void const * d) : l(l), d(d) {}
        Audio(Audio && o) : l(std::move(o.l)), d(std::move(o.d)) {}
        Audio(const Audio & o) : l(o.l), d(o.d) {}
        void const * Data() const;
        uint32_t Length() const;
    private:
        void const * d;
        uint32_t l;
        friend Node;
    };
    class Node {
    public:
        struct Data;
        enum Type : uint16_t {
            none = 0,
            ireal = 1,
            dreal = 2,
            string = 3,
            vector = 4,
            bitmap = 5,
            audio = 6,
        };
        Node() : d(nullptr), f(nullptr) {}
        Node(Node && o) : d(std::move(o.d)), f(std::move(o.f)) {}
        Node(const Node & o) : d(o.d), f(o.f) {}
        Node(Data const * d, File const * f) : d(d), f(f) {}
        Node begin() const;
        Node end() const;
        Node operator*() const;
        Node & operator++();
        Node operator++(int);
        bool operator==(const Node &) const;
        bool operator!=(const Node &) const;
        Node operator[](std::string &&) const;
        Node operator[](const std::string &) const;
        Node operator[](String &&) const;
        Node operator[](const String &) const;
        Node operator[](char *) const;
        Node operator[](char const *) const;
        template <size_t N> Node operator[](char const o[N]) const {return Get(o, N);}
        operator int64_t() const;
        operator double() const;
        operator String() const;
        operator std::string() const;
        operator std::pair<int32_t, int32_t>() const;
        operator Bitmap() const;
        operator Audio() const;
        operator bool() const;
        Node Get(char const *, size_t) const;
        int32_t X() const;
        int32_t Y() const;
        String Name() const;
        size_t Num() const;
        Type T() const;
    private:
        Data const * d;
        File const * f;
        friend File;
    };
    class File {
    public:
        File(char const *);
        ~File();
        Node Base() const;
        uint32_t StringCount() const;
        uint32_t BitmapCount() const;
        uint32_t AudioCount() const;
        uint32_t NodeCount() const;
    private:
        File(const File &) {throw;};
        struct Header;
        void const * base;
        Node::Data const * ntable;
        uint64_t const * stable;
        uint64_t const * btable;
        uint64_t const * atable;
        Header const * head;
#ifdef _WIN32
        void * file;
        void * map;
#elif defined __linux__
        int file;
        size_t size;
#endif
        friend Node;
        friend Bitmap;
        friend Audio;
        friend String;
    };
    class String {
    public:
        String() : d(nullptr) {}
        String(void const * d) : d(d) {}
        String(uint32_t i, File const * f) : d(reinterpret_cast<char const *>(f->base) + f->stable[i]) {}
        uint16_t Size() const;
        char const * Data() const;
        operator std::string() const;
        bool operator==(String) const;
        bool operator!=(String) const;
    private:
        void const * d;
        friend Node;
    };
}
