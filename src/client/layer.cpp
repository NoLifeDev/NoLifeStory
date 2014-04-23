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

#include "layer.hpp"
#include "map.hpp"
#include "player.hpp"
#include <nx/nx.hpp>
#include <algorithm>

namespace nl {
    std::array<layer, 8> layers;
    void layer::render() {
        for (auto i = 0u; i < 8; ++i) {
            for (auto & o : layers[i].objs)
                o.render();
            for (auto & t : layers[i].tiles)
                t.render();
            if (player::ch.pos.layer == i)
                player::render();
        }
    }
    void layer::load() {
        auto tile_node = nx::map["Tile"];
        for (auto i = 0u; i < 8; ++i) {
            auto & l = layers[i];
            auto n = map::current[i];
            auto tn = tile_node[n["info"]["tS"] + ".img"];
            l.objs.clear();
            for (auto nn : n["obj"])
                l.objs.emplace_back(nn);
            std::sort(l.objs.begin(), l.objs.end(), [](obj const & a, obj const & b) {
                if (a.z == b.z)
                    return a.zid < b.zid;
                return a.z < b.z;
            });
            l.tiles.clear();
            for (auto nn : n["tile"])
                l.tiles.emplace_back(nn, tn);
            std::sort(l.tiles.begin(), l.tiles.end(), [](tile const & a, tile const & b) {
                return a.z < b.z;
            });
        }
    }
}
