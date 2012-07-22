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
#include "Global.h"

namespace NL {
    MapFile file;
    uint32_t NodeCount;
    uint64_t NodeOffset;
    uint32_t StringCount;
    uint64_t StringOffset;
    uint32_t SpriteCount;
    uint64_t SpriteOffset;
    uint32_t SoundCount;
    uint64_t SoundOffset;
    void** StringTable;
    Node Base;
#pragma pack(4)
    struct Node::Data {
        uint32_t name;
        uint16_t num;
        Type type;
        union {
            int32_t ireal;
            double dreal;
            uint32_t string;
            int32_t vector[2];
            uint32_t sprite;
            uint32_t sound;
        };
        uint32_t children;
    };
    uint16_t String::Size() {
        if (!d) return 0;
        return *reinterpret_cast<uint16_t*>(d);
    }
    char* String::Data() {
        if (!d) return 0;
        return reinterpret_cast<char*>(d)+2;
    }
    String::operator string() {
        if (!d) return string();
        return string(Data(), Size());
    }
    Node Node::begin() const {
        if (!d) return Node();
        return Base.d + d->children;
    }
    Node Node::end() const {
        if (!d) return Node();
        return Base.d + d->children + d->num;
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
    Node Node::operator[](string o) const {
        return Get(o.c_str(), o.length());
    }
    Node Node::operator[](char* o) const {
        char* e = o;
        for (; *e; ++e);
        return Get(o, e-o);
    }
    Node Node::operator[](const char* o) const {
        const char* e = o;
        for (; *e; ++e);
        return Get(o, e-o);
    }
    Node Node::Get(const char* o, uint16_t l) const {
        if (!d) return Node();
        for (Node n : *this) {
            String s = n.Name();
            if (s.Size() != l) continue;
            const char *i1 = o, *i2 = s.Data(), *i3 = o + l;
            while (true) {
                if (i1 == i3) return n;
                if (*i1 != *i2) break;
                ++i1, ++i2;
            }
        }
        return Node();
    }
    String Node::Name() const {
        String s;
        if (!d) s.d = nullptr;
        else s.d = StringTable[d->name];
        return s;
    }
    Node::Type Node::T() const {
        if (!d) return Type::none;
        return d->type;
    }
    void Load(string filename) {
        file.Open(filename);
        if (file.Read<uint32_t>() != 0x33474B50) throw;
        NodeCount = file.Read<uint32_t>();
        NodeOffset = file.Read<uint64_t>();
        StringCount = file.Read<uint32_t>();
        StringOffset = file.Read<uint64_t>();
        SpriteCount = file.Read<uint32_t>();
        SpriteOffset = file.Read<uint64_t>();
        SoundCount = file.Read<uint32_t>();
        SoundOffset = file.Read<uint64_t>();
        Base.d = reinterpret_cast<Node::Data*>(file.base + NodeOffset);
        StringTable = new void*[StringCount];
        {
            char* ptr = reinterpret_cast<char*>(file.base + StringOffset);
            void** table = StringTable;
            for (uint32_t i = StringCount; i > 0; --i) {
                *table = ptr;
                ptr += *reinterpret_cast<uint16_t*>(ptr)+2;
                ++table;
            }
        }
    }
}