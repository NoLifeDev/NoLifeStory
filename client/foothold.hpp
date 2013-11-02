//////////////////////////////////////////////////////////////////////////////
// NoLifeClient - Part of the NoLifeStory project                           //
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

#pragma once
#include <nx/node.hpp>
#include <cstdint>
#include <vector>

namespace nl {
    class foothold {
    public:
        foothold() = default;
        foothold(foothold const &) = default;
        foothold & operator=(foothold const &) = default;
        foothold(node, uint32_t, uint32_t, uint32_t);
        static void load();
        foothold * next {}, *prev {};
        double x1 {}, y1 {}, x2 {}, y2 {};
        int32_t force {}, piece {};
        uint32_t nextid {}, previd {};
        uint32_t id {}, group {}, layer {};
        bool cant_through {}, forbid_fall_down {};
    private:
    };
    extern std::vector<foothold> footholds;
}