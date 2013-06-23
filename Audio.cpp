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
    Audio::Audio() : d(nullptr), l(0) {}
    Audio::Audio(Audio && o) : d(o.d), l(o.l) {}
    Audio::Audio(Audio const & o) : d(o.d), l(o.l) {}
    Audio::Audio(uint32_t l, void const * d) : d(d), l(l) {}
    Audio & Audio::operator=(Audio o) {
        d = o.d;
        l = o.l;
        return *this;
    }
    bool Audio::operator<(Audio o) const {
        return d < o.d;
    }
    bool Audio::operator==(Audio o) const {
        return d == o.d;
    }
    Audio::operator bool() const {
        return d ? true : false;
    }
    void const * Audio::Data() const {
        return d;
    }
    uint32_t Audio::Length() const {
        return l;
    }
    size_t Audio::ID() const {
        return reinterpret_cast<size_t>(d);
    }
}