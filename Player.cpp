//////////////////////////////////////////////////////////////////////////////
// NoLifeClient - Part of the NoLifeStory project                           //
// Copyright (C) 2013 Peter Atashian                                        //
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
#include "NoLifeClient.hpp"
namespace NL {
    namespace Player {
        Physics Pos;
        uint16_t Level = 69;
        void Respawn(string portal) {
            vector<pair<int32_t, int32_t>> spawns;
            for (auto && p : Portals) {
                if (p.pn == portal) {
                    spawns.emplace_back(p.x, p.y);
                }
            }
            if (!spawns.empty()) {
                auto && spawn = spawns[rand() % spawns.size()];
                Pos.Reset(spawn.first, spawn.second);
            } else {
                Log::Write("Failed to find portal " + portal + " for map " + Map::Name);
                if (portal != "sp") Respawn("sp");
                else Pos.Reset(0, 0);
            }
        }
        void Update() {
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) Pos.left = false;
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) Pos.right = false;
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) Pos.up = false;
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) Pos.down = false;
            Pos.Update();
            for (Portal & p : Portals) {
                if (p.x < Pos.x - 20 || p.x > Pos.x + 20 || p.y < Pos.y - 20 || p.y > Pos.y + 20) continue;
                if (Pos.up && p.tm != 999999999) Map::Load(to_string(p.tm), p.tn);
                switch (p.pt) {//Handle stuff like bouncies here

                }
            }
        }
        void Render() {
            Sprite::Unbind();
            if (!Config::Rave) glColor4f(1, 1, 1, 1);
            Graphics::DrawRect(Pos.x - 20, Pos.y - 60, Pos.x + 20, Pos.y, true);
        }
    }
}