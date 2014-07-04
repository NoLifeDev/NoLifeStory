//////////////////////////////////////////////////////////////////////////////
// NoLifeClient - Part of the NoLifeStory project                           //
// Copyright © 2014 Peter Atashian                                          //
// Additional Authors                                                       //
// 2013 Cedric Van Goethem                                                  //
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

#include "portal.hpp"
#include "map.hpp"
#include <nx/nx.hpp>

namespace nl {
std::vector<portal> portals;
node portal_sprites;
void portal::load() {
    portal_sprites = nx::map["MapHelper.img"]["portal"]["game"];
    portals.clear();
    for (node n : map::current["portal"]) portals.emplace_back(n);
}
portal::portal(node n) : x(n["x"]), y(n["y"]), tm(n["tm"]), tn(n["tn"]), pt(n["pt"]), pn(n["pn"]) {
    switch (pt) {
    case 2:
        spr = portal_sprites["pv"];
        break;
    case 10:
        spr = portal_sprites["ph"]["default"];
        break;
    case 11:
        spr = portal_sprites["psh"]["default"];
        break;
    }
}
void portal::render() {
    switch (pt) {
    case 2:
    case 10:
    case 11:
        spr.draw(x, y, sprite::relative);
        break;
    }
}
}
