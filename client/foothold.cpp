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

#include "foothold.hpp"
#include "map.hpp"
#include <algorithm>
#include <string>

namespace nl {
    std::vector<foothold> footholds;
    foothold::foothold(node n, uint32_t id, uint32_t group, uint32_t layer) : id {id}, group {group}, layer {layer} {
        x1 = n["x1"];
        x2 = n["x2"];
        y1 = n["y1"];
        y2 = n["y2"],
        force = n["force"];
        piece = n["piece"];
        nextid = n["next"];
        previd = n["prev"];
        cant_through = n["cantThrough"].get_bool();
        forbid_fall_down = n["forbidFallDown"].get_bool();
        next = &footholds[nextid];
        prev = &footholds[previd];
    }
    void foothold::load() {
        footholds.clear();
        size_t s = 0;
        for (node layer : map::current["foothold"]) for (node group : layer) for (node id : group) s = std::max<size_t>(std::stoul(id.name()), s);
        footholds.resize(s + 1);
        for (node layer : map::current["foothold"]) for (node group : layer) for (node id : group) {
            footholds[std::stoul(id.name())] = foothold {id, std::stoul(id.name()), std::stoul(group.name()), std::stoul(layer.name())};
        }
    }
}