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
#pragma once
#include "NX.hpp"
namespace NL {
    class Node {
    public:
        enum class Type : uint16_t {
            None = 0,
            Integer = 1,
            Double = 2,
            String = 3,
            Vector = 4,
            Bitmap = 5,
            Audio = 6,
        };
        Node();
        Node(Node &&);
        Node(Node const & o);
        Node & operator=(Node);
        Node begin() const;
        Node end() const;
        Node operator*() const;
        Node & operator++();
        Node operator++(int);
        bool operator==(Node) const;
        bool operator!=(Node) const;
        std::string operator+(std::string const &) const;
        std::string operator+(char const *) const;
        template <typename T>
        typename std::enable_if<std::is_integral<T>::value, Node>::type operator[](T n) const {return operator[](std::to_string(n));}
        Node operator[](std::string const &) const;
        Node operator[](char const *) const;
        Node operator[](Node) const;
        Node operator[](std::pair<char const *, size_t>) const;
        operator int64_t() const;
        operator uint64_t() const;
        operator int32_t() const;
        operator uint32_t() const;
        operator int16_t() const;
        operator uint16_t() const;
        operator int8_t() const;
        operator uint8_t() const;
        operator double() const;
        operator float() const;
        operator std::string() const;
        operator std::pair<int32_t, int32_t>() const;
        operator Bitmap() const;
        operator Audio() const;
        operator bool() const;
        int64_t GetInt() const;
        int64_t GetInt(int64_t) const;
        double GetFloat() const;
        double GetFloat(double) const;
        std::string GetString() const;
        std::pair<int32_t, int32_t> GetVector() const;
        Bitmap GetBitmap() const;
        Audio GetAudio() const;
        bool GetBool() const;
        bool GetBool(bool) const;
        int32_t X() const;
        int32_t Y() const;
        std::string Name() const;
        std::pair<char const *, size_t> NameFast() const;
        size_t Size() const;
        Type T() const;
    private:
#pragma pack(push, 1)
        struct Data {
            uint32_t const name;
            uint32_t const children;
            uint16_t const num;
            Type const type;
            union {
                int64_t const ireal;
                double const dreal;
                uint32_t const string;
                int32_t const vector[2];
                struct {
                    uint32_t index;
                    uint16_t width;
                    uint16_t height;
                } const bitmap;
                struct {
                    uint32_t index;
                    uint32_t length;
                } const audio;
            };
        };
#pragma pack(pop)
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
}