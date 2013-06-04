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
#ifdef _WIN32
#  define NL_WINDOWS
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  define WIN32_LEAN_AND_MEAN
#else
#  define NL_POSIX
#endif
#include <string>
#include <cstdint>
namespace NL {
    class Bitmap;
    class Audio;
    class Node;
    class File;
    class Bitmap {
    public:
        Bitmap() : w(0), h(0), d(nullptr) {}
        Bitmap(Bitmap const & o) : w(o.w), h(o.h), d(o.d) {}
        Bitmap & operator=(Bitmap const &);
        void const * Data() const;
        uint16_t Width() const;
        uint16_t Height() const;
        uint32_t Length() const;
        size_t ID() const;
    private:
        Bitmap(uint16_t w, uint16_t h, void const * d) : w(w), h(h), d(d) {}
        void const * d;
        uint16_t w, h;
        friend Node;
    };
    class Audio {
    public:
        Audio() : l(0), d(nullptr) {}
        Audio(uint32_t l, void const * d) : l(l), d(d) {}
        Audio(Audio const & o) : l(o.l), d(o.d) {}
        Audio & operator=(Audio const &);
        bool operator==(Audio) const;
        operator bool() const;
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
        Node();
        Node(Node const & o);
        Node(Node &&);
        Node & operator=(Node const &);
        Node begin() const;
        Node end() const;
        Node operator*() const;
        Node & operator++();
        Node operator++(int);
        bool operator==(Node) const;
        bool operator!=(Node) const;
        std::string operator+(std::string) const;
        std::string operator+(char const *) const;
        template <typename T>
        typename std::enable_if<std::is_integral<T>::value, Node>::type operator[](T n) const {
            return operator[](std::to_string(n));
        }
        Node operator[](std::string const &) const;
        Node operator[](char const *) const;
        Node operator[](Node) const;
        operator int64_t () const;
        operator uint64_t() const;
        operator int32_t () const;
        operator uint32_t() const;
        operator int16_t () const;
        operator uint16_t() const;
        operator int8_t  () const;
        operator uint8_t () const;
        operator double() const;
        operator float() const;
        operator std::string() const;
        operator std::pair<int32_t, int32_t>() const;
        operator Bitmap() const;
        operator Audio() const;
        operator bool() const;
        int64_t GetInt() const;
        double GetFloat() const;
        std::string GetString() const;
        std::pair<int32_t, int32_t> GetVector() const;
        Bitmap GetBitmap() const;
        Audio GetAudio() const;
        bool GetBool(bool def = false) const;
        int32_t X() const;
        int32_t Y() const;
        std::string Name() const;
        size_t Size() const;
        Type T() const;
    private:
        Node(Data const *, File const *);
        Node GetChild(char const *, size_t) const;
        int64_t ToInt() const;
        double ToFloat() const;
        std::string ToString() const;
        std::pair<int32_t, int32_t> ToVector() const;
        Bitmap ToBitmap() const;
        Audio ToAudio() const;
        Data const * d;
        File const * f;
        friend File;
    };
    std::string operator+(std::string, Node);
    std::string operator+(char const *, Node);
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
        std::string GetString(uint32_t) const;
        File(const File &);
        File & operator=(const File &);
        struct Header;
        void const * base;
        Node::Data const * ntable;
        uint64_t const * stable;
        uint64_t const * btable;
        uint64_t const * atable;
        Header const * head;
#ifdef NL_WINDOWS
        void * file;
        void * map;
#else
        int file;
        size_t size;
#endif
        friend Node;
        friend Bitmap;
        friend Audio;
    };
}
