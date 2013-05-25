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
        vector<string> Maps;
        vector<string>::iterator Cur;
        void Init() {
            for (Node n1 : NXMap["Map"]) {
                for (Node n2 : n1) {
                    string name = n2.Name();
                    if (name.length() == 13) Maps.emplace_back(name.substr(0, name.size() - 4));
                }
            }
            srand(clock());
            random_shuffle(Maps.begin(), Maps.end());
            Cur = Maps.begin();
            Next();
        }
        void Load(string name) {
            name.insert(0, 9 - name.size(), '0');
            Node m = NXMap["Map"][string("Map") + name[0]][name + ".img"];
            if (!m) {
                Log::Write("Failed to load map " + name);
                return;
            }
            Current = m;
            Player::X = 0, Player::Y = 0;
            string bgm = Current["info"]["bgm"];
            if (islower(bgm[0])) bgm[0] = toupper(bgm[0]);
            while (bgm.find(' ') != bgm.npos) bgm.erase(bgm.find(' '), 1);
            size_t p = bgm.find('/');
            Node sn = NXSound[bgm.substr(0, p) + ".img"][bgm.substr(p + 1)];
            if (!sn) Log::Write("Failed to find bgm " + bgm + " for map " + name);
            Music = move(sn);
            Music.Play(true);
            Layer::LoadAll();
            Log::Write("Loaded map " + name);
        }
        void Render() {
            static uint8_t c = 0;
            if (!c++) Next();
            Layer::RenderAll();
        }
        void Next() {
            Load(*Cur++);
            if (Cur == Maps.end()) Cur = Maps.begin();
        }
    }
}