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
    class Bitmap {
    public:
        Bitmap();
        Bitmap(Bitmap &&);
        Bitmap(Bitmap const &);
        Bitmap & operator=(Bitmap);
        bool operator==(Bitmap) const;
        bool operator<(Bitmap) const;
        operator bool() const;
        void const * Data() const;
        uint16_t Width() const;
        uint16_t Height() const;
        uint32_t Length() const;
        size_t ID() const;
    private:
        Bitmap(uint16_t, uint16_t, void const *);
        void const * d;
        uint16_t w, h;
        friend Node;
    };
}