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
    class String {
    public:
        uint16_t Size() const;
        char const * Data() const;
        operator std::string() const;
        bool operator==(String) const;
        bool operator!=(String) const;
        void const * d;
    private:
        static String Construct(void const *);
        static String Construct(uint32_t, File const *);
        static String Blank();
        friend Node;
    };
    class Bitmap {
    public:
        void const * Data() const;
        size_t Width() const;
        size_t Height() const;
        size_t Length() const;
        size_t w, h;
        void const * d;
    private:
        static Bitmap Construct(size_t, size_t, void const *);
        static uint8_t * buf;
        static size_t len;
        friend Node;
    };
    class Audio {
    public:
        void const * Data() const;
        size_t Length() const;
        size_t l;
        void const * d;
    private:
        static Audio Construct(size_t, void const *);
        friend Node;
    };
    class Node {
    public:
        Node begin() const;
        Node end() const;
        Node operator*() const;
        Node operator++();
        Node operator++(int);
        bool operator==(Node) const;
        bool operator!=(Node) const;
        Node operator[](std::string) const;
        Node operator[](String) const;
        Node operator[](char *) const;
        Node operator[](char const *) const;
        template <size_t N> Node operator[](char const o[N]) const {return Construct(Get(o, N), f);}
        operator int64_t() const;
        operator double() const;
        operator String() const;
        operator std::string() const;
        operator std::pair<int32_t, int32_t>() const;
        operator Bitmap() const;
        int32_t X() const;
        int32_t Y() const;
        String Name() const;
        size_t Num() const;
        enum Type : uint16_t {
            none = 0,
            ireal = 1,
            dreal = 2,
            string = 3,
            vector = 4,
            bitmap = 5,
            audio = 6,
        };
        Type T() const;
        struct Data;
        Data const * d;
        File const * f;
    private:
        static Node Construct(Data const *, File const *);
        Data const * Get(char const *, size_t) const;
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
}
