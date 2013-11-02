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
#include <vector>

namespace nl {
    class foothold {
    public:
        foothold(foothold const &) = default;
        foothold() = default;
        foothold & operator=(foothold const &) = default;
        static void load();
        foothold * next = nullptr, * prev = nullptr;
        int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
        int force = 0, piece = 0;
        unsigned nextid = 0, previd = 0;
        unsigned id = 0, group = 0, layer = 0;
        bool cant_through = false, forbid_fall_down= false;
    private:
        foothold(node, unsigned, unsigned, unsigned);
    };
    extern std::vector<foothold> footholds;
}
