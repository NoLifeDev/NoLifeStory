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
        vector<string> Maps;
        void Init() {
            for (int i = 0; i <= 9; ++i) {
                for (Node n2 : NXMap["Map"]["Map" + to_string(i)]) {
                    string name = n2.Name();
                    Maps.emplace_back(name.substr(0, name.size() - 4));
                }
            }
            Next();
        }


        void Load(string name) {
            Load(name, nullptr);
        }

        void Load(string name, string *portal) {
            name.insert(0, 9 - name.size(), '0');
            Node m = NXMap["Map"][string("Map") + name[0]][name + ".img"];
            if (!m) {
                Log::Write("Failed to load map " + name);
                return;
            }
            if (m["info"]["link"]) {
                Load(m["info"]["link"]);
                return;
            }
            Current = m;
            BGM.PlayMusic();
            Foothold::Load();
            Layer::LoadAll();
            Background::Load();
            Portal::Load();
            Sprite::Cleanup();
            Player::Respawn(portal);
            View::Reset();
        }
        void Render() {
            if (Config::Rave) {
                float d = floor(Time::TDelta * 2.088 - 0.1) * 1.95;
                float r(sin(d)), g(sin(d + M_PI * 2 / 3)), b(sin(d + M_PI * 4 / 3));
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
                GLfloat c[] = {r, g, b, 1};
                glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, c);
                glColor4f(1 - r, 1 - g, 1 - b, 1);
            } else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            for (auto && b : Backgrounds) b.Render();
            Layer::RenderAll();
            for (auto && p : NL::Portals) p.Render();
            for (auto && b : Foregrounds) b.Render();
            View::DrawEdges();
        }
        void Next() {
            uniform_int_distribution<size_t> dist(0, Maps.size() - 1);
#ifdef NL_WINDOWS
            mt19937_64 engine(__rdtsc());
#else
            mt19937_64 engine(high_resolution_clock::now().time_since_epoch().count());
#endif
            Load(Maps[dist(engine)]);
        }
    }
}