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

#include "background.hpp"
#include "map.hpp"
#include "view.hpp"
#include "time.hpp"
#include <nx/nx.hpp>

namespace nl {
    std::vector<background> backgrounds;
    std::vector<background> foregrounds;
    background::background(node n) {
        x = n["x"];
        y = n["y"];
        z = n["z"];
        rx = n["rx"];
        ry = n["ry"];
        cx = n["cx"];
        cy = n["cy"];
        type = n["type"];
        flipped = n["f"].get_bool();
        spr = nx::map["Back"][n["bS"] + ".img"]
            [n["ani"].get_bool() ? "ani" : "back"][n["no"]];
    }
    void background::load() {
        backgrounds.clear();
        foregrounds.clear();
        auto b = map::current["back"];
        for (auto i = 0u; b[i]; ++i) {
            auto n = b[i];
            if (n["front"].get_bool())
                foregrounds.emplace_back(n);
            else
                backgrounds.emplace_back(n);
        }
    }
    void background::render() {
        auto dx = x;
        auto dy = y;
        auto flags = sprite::none;
        if (flipped)
            flags |= sprite::flipped;
        switch (type) {
        case 0:
            dx += rx * view::x / 100 + view::width / 2;
            dy += ry * (view::y + (view::height - 600) / 2) / 100 + view::height / 2 + (view::height - 600) / 2;
            break;
        case 1:
            flags |= sprite::tilex;
            dx += rx * view::x / 100 + view::width / 2;
            dy += ry * (view::y + (view::height - 600) / 2) / 100 + view::height / 2 + (view::height - 600) / 2;
            break;
        case 2:
            flags |= sprite::tiley;
            dx += rx * view::x / 100 + view::width / 2;
            dy += ry * (view::y + (view::height - 600) / 2) / 100 + view::height / 2 + (view::height - 600) / 2;
            break;
        case 3:
            flags |= sprite::tilex;
            flags |= sprite::tiley;
            dx += rx * view::x / 100 + view::width / 2;
            dy += ry * (view::y + (view::height - 600) / 2) / 100 + view::height / 2 + (view::height - 600) / 2;
            break;
        case 4:
            flags |= sprite::tilex;
            dx += static_cast<int>(time::delta_total * rx * 10) - view::x;
            dy += ry * (view::y + (view::height - 600) / 2) / 100 + view::height / 2 + (view::height - 600) / 2;
            break;
        case 5:
            flags |= sprite::tiley;
            dx += rx * view::x / 100 + view::width / 2;
            dy += static_cast<int>(time::delta_total * ry * 10) - view::y;
            break;
        case 6:
            flags |= sprite::tilex;
            flags |= sprite::tiley;
            dx += static_cast<int>(time::delta_total * rx * 10) - view::x;
            dy += ry * (view::y + (view::height - 600) / 2) / 100 + view::height / 2 + (view::height - 600) / 2;
            break;
        case 7:
            flags |= sprite::tilex;
            flags |= sprite::tiley;
            dx += rx * view::x / 100 + view::width / 2;
            dy += static_cast<int>(time::delta_total * ry * 10) - view::y;
            break;
        }
        spr.draw(dx, dy, flags, cx, cy);
    }
}
