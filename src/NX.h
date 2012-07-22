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
    class String {
    public:
        uint16_t Size();
        char* Data();
        operator std::string();
        void* d;
    };
    class Sprite {
    };
    class Sound {
    };
    class Node {
    public:
        struct Data;
        enum class Type : uint16_t {
            none = 0,
            ireal = 1,
            dreal = 2,
            string = 3,
            vector = 4,
            sprite = 5,
            sound = 6,
        };
        Node() : d(nullptr) {}
        Node(Data* d) : d(d) {}
        Node(const Node& o) : d(o.d) {}
        Node begin() const;
        Node end() const;
        Node operator*() const;
        Node operator++();
        Node operator++(int);
        bool operator==(Node) const;
        bool operator!=(Node) const;
        Node& operator=(Node);
        Node operator[](std::string) const;
        Node operator[](char*) const;
        Node operator[](const char*) const;
        Node Get(const char*, uint16_t) const;
        String Name() const;
        Type T() const;
        Data* d;
    };
    extern Node Base;
    void Load(std::string);
}