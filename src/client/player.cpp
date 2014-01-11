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

#include "player.hpp"
#include "time.hpp"
#include "portal.hpp"
#include "map.hpp"
#include "config.hpp"
#include "view.hpp"
#include "window.hpp"
#include "log.hpp"
#include <nx/nx.hpp>
#include <nx/node.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>

namespace nl {
    namespace player {
        physics pos;
        double last_tele = time::delta_total;
        sprite spr;
        bool mouse_fly = false;
        void respawn(std::string port) {
            std::vector<std::pair<int, int>> spawns;
            for (auto & p : portals)
                if (p.pn == port)
                    spawns.emplace_back(p.x, p.y);
            if (!spawns.empty()) {
                auto spawn = spawns[rand() % spawns.size()];
                pos.reset(spawn.first, spawn.second - 20);
            } else {
                log << "Failed to find portal " << port << " for map " << map::current_name;
                if (port != "sp")
                    respawn("sp");
                else
                    pos.reset(0, 0);
            }
            spr = nx::mob["1210102.img"]["stand"];
        }
        void update() {
            if (mouse_fly) {
                auto p = window::mouse_pos();
                pos.reset(p.first + view::xmin, p.second + view::ymin);
            }
            if (!window::get_key(GLFW_KEY_RIGHT_SHIFT) && !window::get_key(GLFW_KEY_LEFT_SHIFT))
                mouse_fly = false;
            if (!window::get_key(GLFW_KEY_LEFT))
                pos.left = false;
            if (!window::get_key(GLFW_KEY_RIGHT))
                pos.right = false;
            if (!window::get_key(GLFW_KEY_UP))
                pos.up = false;
            if (!window::get_key(GLFW_KEY_DOWN))
                pos.down = false;
            pos.update();
            if (last_tele + 0.5 > time::delta_total)
                return;
            for (portal & p : portals) {
                if (p.x < pos.x - 40 || p.x > pos.x + 40 || p.y < pos.y - 40 || p.y > pos.y + 40)
                    continue;
                switch (p.pt) {//Handle stuff like bouncies here
                case 1:
                case 2:
                    if (!pos.up)
                        break;
                case 3:
                    map::load(std::to_string(p.tm), p.tn);
                    last_tele = time::delta_total;
                    return;
                }
            }
        }
        void render() {
            auto flags = sprite::relative;
            if (pos.right && !pos.left)
                flags |= sprite::flipped;
            spr.draw(static_cast<int>(pos.x), static_cast<int>(pos.y), flags);
        }
    }
}
