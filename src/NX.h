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
    class Sprite;
    class Sound;
    class Node;
    class File;
    class String {
    public:
        uint16_t Size() const;
        const char * Data() const;
        operator std::string() const;
        bool operator==(String) const;
        bool operator!=(String) const;
    private:
        static String Construct(const void *);
        static String Construct(uint32_t, const File *);
        static String Blank();
        const void * d;
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
        Node operator[](const char *) const;
        operator int64_t() const;
        operator double() const;
        operator String() const;
        operator std::string() const;
        operator std::pair<int32_t, int32_t>() const;
        int32_t X() const;
        int32_t Y() const;
        String Name() const;
        size_t Num() const;
        enum class Type : uint16_t {
            none = 0,
            ireal = 1,
            dreal = 2,
            string = 3,
            vector = 4,
            sprite = 5,
            sound = 6,
        };
        Type T() const;
    private:
        struct Data;
        static Node Construct(const Data *, const File *);
        const Data * Get(const char *, size_t) const;
        const Data * d;
        const File * f;
        friend Sprite;
        friend Sound;
        friend File;
    };
    class File {
    public:
        File(std::string);
        ~File();
        Node Base() const;
    private:
        struct Header;
        const void * base;
        const Node::Data * ntable;
        const uint64_t * stable;
        const Header * head;
#ifdef _WIN32
        void * file;
        void * map;
#elif defined __linux__
        int file;
        size_t size;
#endif
        friend Node;
        friend Sprite;
        friend Sound;
        friend String;
    };
}
