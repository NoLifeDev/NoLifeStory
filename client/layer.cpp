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
#include <nx/nx.hpp>
#include <algorithm>

namespace nl {
    std::array<layer, 8> layers;
    void layer::render() {
        for (uint8_t i = 0; i < 8; ++i) {
            //for (obj & o : layers[i].Objs) o.Render();
            for (tile & t : layers[i].tiles) t.render();
            //if (Player::Pos.layer == i) Player::Render();
        }
    }
    void layer::load() {
        for (uint8_t i = 0; i < 8; ++i) {
            layer & l = layers[i];
            node n = map::current[i];
            node tn = nx::map["Tile"][n["info"]["tS"] + ".img"];
            //l.Objs.clear();
            //for (node nn : n["obj"]) l.Objs.emplace_back(nn);
            //sort(l.Objs.begin(), l.Objs.end());
            l.tiles.clear();
            for (node nn : n["tile"]) l.tiles.emplace_back(nn, tn);
            std::sort(l.tiles.begin(), l.tiles.end(), [](tile const & a, tile const & b) {
                return a.z < b.z;
            });
        }
    }
}
