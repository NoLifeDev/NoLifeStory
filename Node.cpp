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
#include "NX.hpp"
namespace NL {
    Node::Node() : d(nullptr), f(nullptr) {}
    Node::Node(Node && o) : d(o.d), f(o.f) {}
    Node::Node(Node const & o) : d(o.d), f(o.f) {}
    Node::Node(Data const * d, File const * f) : d(d), f(f) {}
    Node & Node::operator=(Node o) {
        d = o.d;
        f = o.f;
        return *this;
    }
    Node Node::begin() const {
        if (d) return Node(f->ntable + d->children, f);
        return Node();
    }
    Node Node::end() const {
        if (d) return Node(f->ntable + d->children + d->num, f);
        return Node();
    }
    Node Node::operator*() const {
        return *this;
    }
    Node & Node::operator++() {
        ++d;
        return *this;
    }
    Node Node::operator++(int) {
        return Node(d++, f);
    }
    bool Node::operator==(Node o) const {
        return d == o.d;
    }
    bool Node::operator!=(Node o) const {
        return d != o.d;
    }
    std::string operator+(std::string s, Node n) {
        return move(s) + n.GetString();
    }
    std::string operator+(char const * s, Node n) {
        return s + n.GetString();
    }
    std::string Node::operator+(std::string const & s) const {
        return GetString() + s;
    }
    std::string Node::operator+(char const * s) const {
        return GetString() + s;
    }
    Node Node::operator[](std::string const & o) const {
        return GetChild(o.c_str(), o.length());
    }
    Node Node::operator[](char const * o) const {
        return GetChild(o, strlen(o));
    }
    Node Node::operator[](Node o) const {
        return operator[](o.GetString());
    }
    Node Node::operator[](std::pair<char const *, size_t> o) const {
        return GetChild(o.first, o.second);
    }
    Node::operator int64_t() const {
        return static_cast<int64_t>(GetInt());
    }
    Node::operator uint64_t() const {
        return static_cast<uint64_t>(GetInt());
    }
    Node::operator int32_t() const {
        return static_cast<int32_t >(GetInt());
    }
    Node::operator uint32_t() const {
        return static_cast<uint32_t>(GetInt());
    }
    Node::operator int16_t() const {
        return static_cast<int16_t>(GetInt());
    }
    Node::operator uint16_t() const {
        return static_cast<uint16_t>(GetInt());
    }
    Node::operator int8_t () const {
        return static_cast<int8_t>(GetInt());
    }
    Node::operator uint8_t() const {
        return static_cast<uint8_t>(GetInt());
    }
    Node::operator double() const {
        return static_cast<double>(GetFloat());
    }
    Node::operator float() const {
        return static_cast<float>(GetFloat());
    }
    Node::operator std::string() const {
        return GetString();
    }
    Node::operator std::pair<int32_t, int32_t>() const {
        return GetVector();
    }
    Node::operator Bitmap() const {
        return GetBitmap();
    }
    Node::operator Audio() const {
        return GetAudio();
    }
    Node::operator bool() const {
        return d ? true : false;
    }
    int64_t Node::GetInt() const {
        return GetInt(0);
    }
    int64_t Node::GetInt(int64_t def) const {
        if (d) switch (d->type) {
        case Type::None: return def;
        case Type::Integer: return ToInt();
        case Type::Double: return static_cast<int64_t>(ToFloat());
        case Type::String: return std::stoll(ToString());
        case Type::Vector: return def;
        case Type::Bitmap: return def;
        case Type::Audio: return def;
        default: throw "Unknown Node type";
        }
        return def;
    }
    double Node::GetFloat() const {
        return GetFloat(0);
    }
    double Node::GetFloat(double def) const {
        if (d) switch (d->type) {
        case Type::None: return def;
        case Type::Integer: return static_cast<double>(ToInt());
        case Type::Double: return ToFloat();
        case Type::String: return std::stod(ToString());
        case Type::Vector: return def;
        case Type::Bitmap: return def;
        case Type::Audio: return def;
        default: throw "Unknown Node type";
        }
        return def;
    }
    std::string Node::GetString() const {
        if (d) switch (d->type) {
        case Type::None: return std::string();
        case Type::Integer: return std::to_string(ToInt());
        case Type::Double: return std::to_string(ToFloat());
        case Type::String: return ToString();
        case Type::Vector: return "(" + std::to_string(d->vector[0]) + ", " + std::to_string(d->vector[1]) + ")";
        case Type::Bitmap: return "Bitmap";
        case Type::Audio: return "Audio";
        default: return std::string();
        }
        return std::string();
    }
    std::pair<int32_t, int32_t> Node::GetVector() const {
        if (d) if (d->type == Type::Vector) return ToVector();
        return std::make_pair(0, 0);
    }
    Bitmap Node::GetBitmap() const {
        if (d) if (d->type == Type::Bitmap) return ToBitmap();
        return Bitmap();
    }
    Audio Node::GetAudio() const {
        if (d) if (d->type == Type::Audio) return ToAudio();
        return Audio();
    }
    bool Node::GetBool() const {
        if (d) if (d->type == Type::Integer) return ToInt() ? true : false;
        return false;
    }
    bool Node::GetBool(bool def) const {
        if (d) if (d->type == Type::Integer) return ToInt() ? true : false;
        return def;
    }
    int32_t Node::X() const {
        if (d) if (d->type == Type::Vector) return d->vector[0];
        return 0;
    }
    int32_t Node::Y() const {
        if (d) if (d->type == Type::Vector) return d->vector[1];
        return 0;
    }
    std::string Node::Name() const {
        if (d) return f->GetString(d->name);
        return std::string();
    }
    std::pair<char const *, size_t> Node::NameFast() const {
        if (!d) return std::make_pair(nullptr, 0);
        char const * s = reinterpret_cast<char const *>(f->base) + f->stable[d->name];
        return std::make_pair(s + 2, *reinterpret_cast<uint16_t const *>(s));
    }
    size_t Node::Size() const {
        if (d) return d->num;
        return 0;
    }
    Node::Type Node::T() const {
        if (d) return d->type;
        return Type::None;
    }
    Node Node::GetChild(char const * const o, size_t const l) const {
        if (!d) return Node();
        Data const * p = f->ntable + d->children;
        size_t n = d->num;
        char const * const b = reinterpret_cast<const char *>(f->base);
        uint64_t const * const t = f->stable;
        for (;;) {
            if (!n) return Node();
            size_t const n2 = n >> 1;
            Data const * const p2 = p + n2;
            char const * const sl = b + t[p2->name];
            size_t const l1 = *reinterpret_cast<uint16_t const *>(sl);
            uint8_t const * s = reinterpret_cast<uint8_t const *>(sl + 2);
            uint8_t const * os = reinterpret_cast<uint8_t const *>(o);
            size_t const al = l1 < l ? l1 : l;
            size_t i = al;
            bool z = false;
            for (; i; --i, ++s, ++os) {
                if (*s > *os) {
                    n = n2;
                    z = true;
                    break;
                } else if (*s < *os) {
                    p = p2 + 1;
                    n -= n2 + 1;
                    z = true;
                    break;
                }
            }
            if (z) continue;
            else if (l1 < l) p = p2 + 1, n -= n2 + 1;
            else if (l1 > l) n = n2;
            else return Node(p2, f);
        }
    }
    int64_t Node::ToInt() const {
        return d->ireal;
    }
    double Node::ToFloat() const {
        return d->dreal;
    }
    std::string Node::ToString() const {
        return f->GetString(d->string);
    }
    std::pair<int32_t, int32_t> Node::ToVector() const {
        return std::make_pair(d->vector[0], d->vector[1]);
    }
    Bitmap Node::ToBitmap() const {
        return Bitmap(d->bitmap.width, d->bitmap.height, reinterpret_cast<char const *>(f->base) + f->btable[d->bitmap.index]);
    }
    Audio Node::ToAudio() const {
        return Audio(d->audio.length, reinterpret_cast<char const *>(f->base) + f->atable[d->audio.index]);
    }
}