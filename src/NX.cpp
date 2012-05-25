//////////////////////////////////////////////////////////////////////////
// Copyright 2012 Peter Atechian (Retep998)                             //
//////////////////////////////////////////////////////////////////////////
// This file is part of NoLifeNx.                                       //
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
        enum class t : uint8_t {
            none = 0,
            ireal = 1,
            dreal = 2,
            string = 3,
            vector = 4,
            sprite = 5,
            sound = 6,
            link = 7
        } type;
    };
    Node::Data* Node::operator->() {
        return d;
    }
    void LoadStrings() {
        file.Seek(StringOffset);
        for (uint32_t i = 0; i < StringCount; ++i) {
            StringTable[i] = file.TellPtr();
            file.Skip(file.Read<uint16_t>());
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
        n->type = static_cast<Node::Data::t>(type);
        switch (type) {
        case Node::Data::t::ireal:
            n->ireal = file.Read<int32_t>();
            break;
        case Node::Data::t::dreal:
            n->dreal = file.Read<double>();
            break;
        case Node::Data::t::string:
            n->string = StringTable[file.Read<uint32_t>()];
            break;
        case Node::Data::t::vector:
            n->vector[0] = file.Read<int32_t>();
            n->vector[1] = file.Read<int32_t>();
            break;
        case Node::Data::t::sprite:
            file.Read<uint32_t>();
            //NodeTable[i].sprite = SpriteTable[file.Read<uint32_t>()];
            break;
        case Node::Data::t::sound:
            file.Read<uint32_t>();
            //NodeTable[i].sound = SoundTable[file.Read<uint32_t>()];
            break;
        case Node::Data::t::link:
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
        LoadNodes();
    }
}