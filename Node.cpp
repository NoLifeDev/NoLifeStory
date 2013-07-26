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
#include <cstring>
namespace NL {
#ifdef NL_NODE_CONSTRUCTORS
    Node::Node() : d {nullptr}, f {nullptr} {}
    Node::Node(Node && o) : d {std::move(o.d)}, f {std::move(o.f)} {}
    Node::Node(Node const & o) : d {o.d}, f {o.f} {}
    Node::Node(Data const * d, File const * f) : d {d}, f {f} {}
    Node & Node::operator=(Node const & o) {
        return d = o.d, f = o.f, *this;
    }
#endif
    Node Node::begin() const {
        return {d ? f->ntable + d->children : nullptr, f};
    }
    Node Node::end() const {
        return {d ? f->ntable + d->children + d->num : nullptr, f};
    }
    Node Node::operator*() const {
        return *this;
    }
    Node & Node::operator++() {
        return ++d, *this;
    }
    Node Node::operator++(int) {
        return {d++, f};
    }
    bool Node::operator==(Node o) const {
        return d == o.d;
    }
    bool Node::operator!=(Node o) const {
        return d != o.d;
    }
    std::string operator+(std::string s, Node n) {
        return std::move(s) + n.GetString();
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
    template <typename Z>
    typename std::enable_if<std::is_integral<Z>::value, Node>::type Node::operator[](Z n) const {
        return operator[](std::to_string(n));
    }
    template Node Node::operator[]<char>(char)const;
    template Node Node::operator[]<unsigned char>(unsigned char)const;
    template Node Node::operator[]<signed char>(signed char)const;
    template Node Node::operator[]<unsigned short>(unsigned short)const;
    template Node Node::operator[]<signed short>(signed short)const;
    template Node Node::operator[]<unsigned int>(unsigned int)const;
    template Node Node::operator[]<signed int>(signed int)const;
    template Node Node::operator[]<unsigned long>(unsigned long)const;
    template Node Node::operator[]<signed long>(signed long)const;
    template Node Node::operator[]<unsigned long long>(unsigned long long)const;
    template Node Node::operator[]<signed long long>(signed long long)const;
    Node Node::operator[](std::string const & o) const {
        return GetChild(o.c_str(), o.length());
    }
    Node Node::operator[](char const * o) const {
        return GetChild(o, std::strlen(o));
    }
    Node Node::operator[](Node o) const {
        return operator[](o.GetString());
    }
    Node Node::operator[](std::pair<char const *, size_t> o) const {
        return GetChild(o.first, o.second);
    }
    Node::operator unsigned char() const {
        return static_cast<unsigned char>(GetInt());
    }
    Node::operator signed char() const {
        return static_cast<signed char>(GetInt());
    }
    Node::operator unsigned short() const {
        return static_cast<unsigned short>(GetInt());
    }
    Node::operator signed short() const {
        return static_cast<signed short>(GetInt());
    }
    Node::operator unsigned int() const {
        return static_cast<unsigned int>(GetInt());
    }
    Node::operator signed int() const {
        return static_cast<signed int>(GetInt());
    }
    Node::operator unsigned long() const {
        return static_cast<unsigned long>(GetInt());
    }
    Node::operator signed long() const {
        return static_cast<signed long>(GetInt());
    }
    Node::operator unsigned long long() const {
        return static_cast<unsigned long long>(GetInt());
    }
    Node::operator signed long long() const {
        return static_cast<signed long long>(GetInt());
    }
    Node::operator float() const {
        return static_cast<float>(GetFloat());
    }
    Node::operator double() const {
        return static_cast<double>(GetFloat());
    }
    Node::operator long double() const {
        return static_cast<long double>(GetFloat());
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
        case Type::Int: return ToInt();
        case Type::Float: return static_cast<int64_t>(ToFloat());
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
        case Type::Int: return static_cast<double>(ToInt());
        case Type::Float: return ToFloat();
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
        case Type::None: return std::string {};
        case Type::Int: return std::to_string(ToInt());
        case Type::Float: return std::to_string(ToFloat());
        case Type::String: return ToString();
        case Type::Vector: return "(" + std::to_string(d->vector[0]) + ", " + std::to_string(d->vector[1]) + ")";
        case Type::Bitmap: return "Bitmap";
        case Type::Audio: return "Audio";
        default: throw "Unknown Node type";
        }
        return std::string();
    }
    std::pair<int32_t, int32_t> Node::GetVector() const {
        return d && d->type == Type::Vector ? ToVector() : std::pair<int32_t, int32_t> {0, 0};
    }
    Bitmap Node::GetBitmap() const {
        return d && d->type == Type::Bitmap ? ToBitmap() : Bitmap {nullptr, 0, 0};
    }
    Audio Node::GetAudio() const {
        return d && d->type == Type::Audio ? ToAudio() : Audio {nullptr, 0};
    }
    bool Node::GetBool() const {
        return d && d->type == Type::Int && ToInt() ? true : false;
    }
    bool Node::GetBool(bool def) const {
        return d && d->type == Type::Int ? ToInt() ? true : false : def;
    }
    int32_t Node::X() const {
        return d && d->type == Type::Vector ? d->vector[0] : 0;
    }
    int32_t Node::Y() const {
        return d && d->type == Type::Vector ? d->vector[1] : 0;
    }
    std::string Node::Name() const {
        return !d ? std::string {} : f->GetString(d->name);
    }
    std::pair<char const *, size_t> Node::NameFast() const {
        if (!d) return {nullptr, 0};
        char const * s {reinterpret_cast<char const *>(f->base) + f->stable[d->name]};
        return {s + 2, *reinterpret_cast<uint16_t const *>(s)};
    }
    size_t Node::Size() const {
        return d ? d->num : 0U;
    }
    Node::Type Node::T() const {
        return d ? d->type : Type::None;
    }
    Node Node::GetChild(char const * const o, size_t const l) const {
        if (!d) return {nullptr, f};
        Data const * p {f->ntable + d->children};
        size_t n {d->num};
        char const * const b {reinterpret_cast<const char *>(f->base)};
        uint64_t const * const t {f->stable};
        for (;;) {
            if (!n) return {nullptr, f};
            size_t const n2 {n >> 1};
            Data const * const p2 {p + n2};
            char const * const sl {b + t[p2->name]};
            size_t const l1 {*reinterpret_cast<uint16_t const *>(sl)};
            uint8_t const * s {reinterpret_cast<uint8_t const *>(sl + 2)};
            uint8_t const * os {reinterpret_cast<uint8_t const *>(o)};
            bool z {false};
            for (size_t i {l1 < l ? l1 : l}; i; --i, ++s, ++os) {
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
            else return {p2, f};
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
        return {d->vector[0], d->vector[1]};
    }
    Bitmap Node::ToBitmap() const {
        return {reinterpret_cast<char const *>(f->base) + f->btable[d->bitmap.index], d->bitmap.width, d->bitmap.height};
    }
    Audio Node::ToAudio() const {
        return {reinterpret_cast<char const *>(f->base) + f->atable[d->audio.index], d->audio.length};
    }
}