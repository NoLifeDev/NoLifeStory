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
    namespace Map {
        Node Current;
        Sound Music;
        void Load(string name) {
            name.insert(0, 9 - name.size(), '0');
            Node m = NXMap["Map"][string("Map") + name[0]][name + ".img"];
            if (!m) {
                Log::Write("Failed to load map " + name);
                return;
            }
            Current = m;
            string bgm = Current["info"]["bgm"];
            size_t p = bgm.find('/');
            if (p == bgm.npos) {
                Log::Write("Failed to find bgm for map");
                throw;
            }
            Music = NXSound[bgm.substr(0, p) + ".img"][bgm.substr(p + 1)];
            Music.Play(true);
            Layer::LoadAll();
            Log::Write("Loaded map " + name);
        }
        void Render() {
            Layer::RenderAll();
        }
    }
}