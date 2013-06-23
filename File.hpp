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
    class File {
    public:
        File(char const *);
        ~File();
        Node Base() const;
        operator Node() const;
        uint32_t StringCount() const;
        uint32_t BitmapCount() const;
        uint32_t AudioCount() const;
        uint32_t NodeCount() const;
    private:
#pragma pack(push, 1)
        struct Header {
            uint32_t const magic;
            uint32_t const ncount;
            uint64_t const noffset;
            uint32_t const scount;
            uint64_t const soffset;
            uint32_t const bcount;
            uint64_t const boffset;
            uint32_t const acount;
            uint64_t const aoffset;
        };
#pragma pack(pop)
        std::string GetString(uint32_t) const;
        File(const File &);
        File & operator=(const File &);
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