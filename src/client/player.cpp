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
#include <cstdlib>
#include <iostream>

namespace nl {
    namespace player {
        physics pos;
        uint16_t level = 69;
        double last_tele = time::delta_total;
        void respawn(std::string portal) {
            std::vector<std::pair<int32_t, int32_t>> spawns;
            for (auto && p : portals) {
                if (p.pn == portal)
                    spawns.emplace_back(p.x, p.y);
            }
            if (!spawns.empty()) {
                auto && spawn = spawns[rand() % spawns.size()];
                pos.reset(spawn.first, spawn.second - 20);
            } else {
                std::cerr << "Failed to find portal " << portal << " for map " << map::current_name;
                if (portal != "sp")
                    respawn("sp");
                else
                    pos.reset(0, 0);
            }
        }
        void update() {
            /*if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                pos.left = false;
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                pos.right = false;
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                pos.up = false;
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                pos.down = false;*/
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
            /*Sprite::Unbind();
            if (!config::rave)
                glColor4f(1, 1, 1, 1);
            Graphics::DrawRect(pos.x - 20, pos.y - 60, pos.x + 20, pos.y, true);*/
        }
    }
}
