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

        void Respawn() {
            Respawn(nullptr);
        }

        void Respawn(string *portal) {
            int32_t x, y;
            bool found = false;

            if(portal != nullptr) {
                for (auto && p : Portals) {
                    if (p.pn == *portal) {
                        x = p.x;
                        y = p.y;
                        found = true;
                        break;
                    }
                }
            }

            if(!found) { // When the portal isn't found, we choose a spawnpoint randomly
                vector<pair<int32_t, int32_t>> spawns;
                for (auto && p : Portals) {
                    if (p.pn == "sp") {
                        spawns.emplace_back(p.x, p.y);
                    }
                }

                if (!spawns.empty()) {
                    auto && spawn = spawns[rand() % spawns.size()];
                    x = spawn.first;
                    y = spawn.second;
                } else {
                    Log::Write("Map " + Map::Current.Name() + " has no spawn");
                    x = 0;
                    y = 0;
                }
            } 
            Pos.Reset(x, y);
        }
        void Update() {
            Pos.Update();
            for (auto && p : Portals) { //TODO: reset physics such as float/walking speed
                if(p.Check())
                    break;
            }
        }
        void Render() {
            Sprite::Unbind();
            if (!Config::Rave) glColor4f(1, 1, 1, 1);
            Graphics::DrawRect(Pos.x - 20, Pos.y - 60, Pos.x + 20, Pos.y, true);
        }
    }
}