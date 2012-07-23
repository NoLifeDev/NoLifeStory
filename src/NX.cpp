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
    void** StringTable = nullptr;
    Node::Data* NodeTable;
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
    uint16_t String::Size() const {
        if (!d) return 0;
        return *reinterpret_cast<uint16_t*>(d);
    }
    char* String::Data() const {
        if (!d) return 0;
        return reinterpret_cast<char*>(d)+2;
    }
    String::operator string() const {
        if (!d) return string();
        return string(Data(), Size());
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
    Node Node::operator[](string o) const {
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
            char *s = (char*)StringTable[nd->name];
            if (*(uint16_t*)s != l) continue;
            const char *i1 = o, *i2 = s+2;
            if (memcmp(i1, i2, l) == 0) return nd;
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
        else s.d = StringTable[d->name];
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
    void Load(string filename) {
        file.Open(filename);
        for (size_t i = 1000; i > 0; --i) {
            delete StringTable;
            file.Seek(0);
            if (file.Read<uint32_t>() != 0x33474B50) throw;
            NodeCount = file.Read<uint32_t>();
            NodeOffset = file.Read<uint64_t>();
            StringCount = file.Read<uint32_t>();
            StringOffset = file.Read<uint64_t>();
            SpriteCount = file.Read<uint32_t>();
            SpriteOffset = file.Read<uint64_t>();
            SoundCount = file.Read<uint32_t>();
            SoundOffset = file.Read<uint64_t>();
            NodeTable = reinterpret_cast<Node::Data*>(file.base + NodeOffset);
            Base.d = NodeTable;
            StringTable = new void*[StringCount];
            {
                char* ptr = reinterpret_cast<char*>(file.base + StringOffset);
                void** table = StringTable;
                for (size_t i = StringCount; i > 0; --i) {
                    *table = ptr;
                    ptr += *reinterpret_cast<uint16_t*>(ptr);
                    ptr += 2;
                    ++table;
                }
            }
        }
    }
}