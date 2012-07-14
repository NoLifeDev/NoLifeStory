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
    Node::Data** NodeTable;
    uint32_t StringCount;
    uint64_t StringOffset;
    void** StringTable;
    uint32_t SpriteCount;
    uint64_t SpriteOffset;
    void** SpriteTable;
    uint32_t SoundCount;
    uint64_t SoundOffset;
    void** SoundTable;
    Node Base;
    struct Node::Data {
        Data* parent;
        void* name;
        union {
            int32_t ireal;
            double dreal;
            void* string;
            int32_t vector[2];
            Sprite sprite;
            Sound sound;
            Data* link;
        };
        Data* children;
        uint16_t num;
        Type type;
    };
    uint16_t String::Size() {
        if (!d) return 0;
        return *(uint16_t*)d;
    }
    char* String::Data() {
        if (!d) return 0;
        return (char*)d+2;
    }
    String::operator string() {
        if (!d) return string();
        return string(Data(), Size());
    }
    Node Node::begin() {
        if (!d) return Node();
        return d->children;
    }
    Node Node::end() {
        if (!d) return Node();
        return d->children + d->num;
    }
    Node Node::operator*() {
        return *this;
    }
    Node Node::operator++() {
        return ++d;
    }
    Node Node::operator++(int) {
        return d++;
    }
    bool Node::operator==(Node o) {
        return d == o.d;
    }
    bool Node::operator!=(Node o) {
        return d != o.d;
    }
    String Node::Name() {
        String s;
        if (!d) s.d = nullptr;
        else s.d = d->name;
        return s;
    }
    Node::Type Node::T() {
        if (!d) return Type::none;
        return d->type;
    }
    void LoadStrings() {
        file.Seek(StringOffset);
        for (uint32_t i = 0; i < StringCount; ++i) {
            StringTable[i] = file.TellPtr();
            file.Skip(file.Read<uint16_t>());
        }
    }
    void LoadSprites() {
        for (uint32_t i = 0; i < SpriteCount; ++i) {
            SpriteTable[i] = file.base+file.Read<uint64_t>();
        }
    }
    void LoadSounds() {
        for (uint32_t i = 0; i < SoundCount; ++i) {
            SoundTable[i] = file.base+file.Read<uint64_t>();
        }
    }
    Node::Data* buf;
    int bi = 0;
    int i = 0;
    void ParseNode() {
        Node::Data* n = NodeTable[i];
        uint32_t nid = file.Read<uint32_t>();
        n->name = StringTable[nid];
        uint8_t type = file.Read<uint8_t>();
        bool haschildren = !!(type & 0x80);
        type = type & 0x7F;
        n->type = static_cast<Node::Type>(type);
        switch (type) {
        case Node::Type::ireal:
            n->ireal = file.Read<int32_t>();
            break;
        case Node::Type::dreal:
            n->dreal = file.Read<double>();
            break;
        case Node::Type::string:
            n->string = StringTable[file.Read<uint32_t>()];
            break;
        case Node::Type::vector:
            n->vector[0] = file.Read<int32_t>();
            n->vector[1] = file.Read<int32_t>();
            break;
        case Node::Type::sprite:
            file.Read<uint32_t>();
            //NodeTable[i].sprite = SpriteTable[file.Read<uint32_t>()];
            break;
        case Node::Type::sound:
            file.Read<uint32_t>();
            //NodeTable[i].sound = SoundTable[file.Read<uint32_t>()];
            break;
        case Node::Type::link:
            n->link = NodeTable[file.Read<uint32_t>()];
            break;
        }
        if (haschildren) {
            uint16_t num = file.Read<uint16_t>();
            n->num = num;
            int b = bi+1;
            n->children = &buf[b];
            bi += num;
            for (uint16_t j = 0; j < num; ++j) {
                ++i;
                NodeTable[i] = &buf[b+j];
                NodeTable[i]->parent = n;
                ParseNode();
            }
        } else {
            n->num = 0;
            n->children = nullptr;
        }
    }
    void LoadNodes() {
        buf = new Node::Data[NodeCount];
        NodeTable[i] = &buf[bi];
        NodeTable[i]->parent = NodeTable[i];
        file.Seek(NodeOffset);
        ParseNode();
        Base.d = NodeTable[0];
        char* s = new char[6];
        memcpy(s+2, "Base", 4);
        *(uint16_t*)s = 4;
        Base.d->name = s;
    }
    void Load(string filename) {
        file.Open(filename);
        string magic = file.ReadString(4);
        if (magic != "PKG2") die();
        NodeCount = file.Read<uint32_t>();
        NodeOffset = file.Read<uint64_t>();
        NodeTable = new Node::Data*[NodeCount];
        StringCount = file.Read<uint32_t>();
        StringOffset = file.Read<uint64_t>();
        StringTable = new void*[StringCount];
        SpriteCount = file.Read<uint32_t>();
        SpriteOffset = file.Read<uint64_t>();
        SpriteTable = new void*[SpriteCount];
        SoundCount = file.Read<uint32_t>();
        SoundOffset = file.Read<uint64_t>();
        SoundTable = new void*[SoundCount];
        LoadStrings();
        LoadSprites();
        LoadSounds();
        LoadNodes();
    }
}