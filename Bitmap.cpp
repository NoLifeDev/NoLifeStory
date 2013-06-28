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
#include "lz4.hpp"
#include <vector>
namespace NL {
    bool Bitmap::operator < (Bitmap o) const {
        return d < o.d;
    }
    bool Bitmap::operator==(Bitmap o) const {
        return d == o.d;
    }
    Bitmap::operator bool() const {
        return d ? true : false;
    }
    std::vector<uint8_t> buf;
    void const * Bitmap::Data() const {
        if (!d) return nullptr;
        size_t const l {Length() + 0x20};
        if (l > buf.size()) buf.resize(Length() + 0x20);
        LZ4::Uncompress(reinterpret_cast<uint8_t const *>(d) + 4, buf.data(), Length());
        return buf.data();
    }
    uint16_t Bitmap::Width() const {
        return w;
    }
    uint16_t Bitmap::Height() const {
        return h;
    }
    uint32_t Bitmap::Length() const {
        return 4u * w * h;
    }
    size_t Bitmap::ID() const {
        return reinterpret_cast<size_t>(d);
    }
}