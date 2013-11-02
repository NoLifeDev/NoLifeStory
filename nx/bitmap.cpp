//////////////////////////////////////////////////////////////////////////////
// NoLifeNx - Part of the NoLifeStory project                               //
// Copyright © 2013 Peter Atashian                                          //
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

#include "bitmap.hpp"
#include "lz4.hpp"
#include <vector>

namespace nl {
    bool bitmap::operator<(bitmap const & o) const {
        return m_data < o.m_data;
    }
    bool bitmap::operator==(bitmap const & o) const {
        return m_data == o.m_data;
    }
    bitmap::operator bool() const {
        return m_data ? true : false;
    }
    std::vector<uint8_t> buf {};
    void const * bitmap::data() const {
        if (!m_data) return nullptr;
        size_t const l {length()};
        if (l + 0x20 > buf.size()) buf.resize(l + 0x20);
        lz4::uncompress(reinterpret_cast<uint8_t const *>(m_data) + 4, buf.data(), l);
        return buf.data();
    }
    uint16_t bitmap::width() const {
        return m_width;
    }
    uint16_t bitmap::height() const {
        return m_height;
    }
    uint32_t bitmap::length() const {
        return 4u * m_width * m_height;
    }
    size_t bitmap::id() const {
        return reinterpret_cast<size_t>(m_data);
    }
}
