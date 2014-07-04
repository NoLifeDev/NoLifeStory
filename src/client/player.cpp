//////////////////////////////////////////////////////////////////////////////
// NoLifeClient - Part of the NoLifeStory project                           //
// Copyright © 2014 Peter Atashian                                          //
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
#include "character.hpp"
#include <nx/nx.hpp>
#include <nx/node.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>

namespace nl {
namespace player {
double last_tele = time::delta_total;
bool mouse_fly = false;
character ch;
void init() {
    ch.m_parts.emplace_back(2000);
    ch.m_parts.emplace_back(12000);
    ch.m_parts.emplace_back(30000);
    ch.m_parts.emplace_back(1000000);
    ch.m_parts.emplace_back(1050000);
    ch.m_parts.emplace_back(1070000);
    ch.m_parts.emplace_back(1080000);
    ch.m_parts.emplace_back(1090000);
    ch.m_parts.emplace_back(1100000);
}
void respawn(std::string port) {
    last_tele = time::delta_total;
    std::vector<std::pair<int, int>> spawns;
    for (auto &p : portals)
        if (p.pn == port) spawns.emplace_back(p.x, p.y);
    if (!spawns.empty()) {
        auto spawn = spawns[rand() % spawns.size()];
        ch.pos.reset(spawn.first, spawn.second - 20);
    } else {
        log << "Failed to find portal " << port << " for map " << map::current_name;
        if (port != "sp")
            respawn("sp");
        else
            ch.pos.reset(0, 0);
    }
}
void update() {
    if (mouse_fly) {
        auto p = window::mouse_pos();
        ch.pos.reset(p.first + view::xmin, p.second + view::ymin);
    }
    if (!window::get_key(GLFW_KEY_RIGHT_SHIFT) && !window::get_key(GLFW_KEY_LEFT_SHIFT))
        mouse_fly = false;
    if (!window::get_key(GLFW_KEY_LEFT)) ch.pos.left = false;
    if (!window::get_key(GLFW_KEY_RIGHT)) ch.pos.right = false;
    if (!window::get_key(GLFW_KEY_UP)) ch.pos.up = false;
    if (!window::get_key(GLFW_KEY_DOWN)) ch.pos.down = false;
    ch.update();
    if (time::delta_total < last_tele + 0.5) return;
    for (portal &p : portals) {
        if (p.x < ch.pos.x - 40 || p.x > ch.pos.x + 40 || p.y < ch.pos.y - 40
            || p.y > ch.pos.y + 40)
            continue;
        switch (p.pt) { // Handle stuff like bouncies here
        case 1:
        case 2:
            if (!ch.pos.up) break;
        case 3:
            map::load(std::to_string(p.tm), p.tn);
            return;
        }
    }
}
void render() { ch.render(); }
}
}
