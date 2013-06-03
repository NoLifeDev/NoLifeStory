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
            for (int i = 1; i < 9; ++i) {
                for (Node n2 : NXMap["Map"]["Map" + to_string(i)]) {
                    string name = n2.Name();
                    Maps.emplace_back(name.substr(0, name.size() - 4));
                }
            }
            if (Mindfuck) {
                BGM.LoadFile("bgm.mp3");
                BGM.setLoop(true);
                BGM.play();
            }
            Next();
        }
        void Load(string name) {
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
            vector<pair<int32_t, int32_t>> spawns;
            for (Node n : Current["portal"]) {
                if ((string)n["pn"] == "sp") {
                    spawns.emplace_back(n["x"], n["y"]);
                }
            }
            if (!spawns.empty()) {
                auto spawn = spawns[rand() % spawns.size()];
                Player::Pos.x = spawn.first;
                Player::Pos.y = spawn.second;
            } else {
                Log::Write("Map " + name + " has no spawn");
                Player::Pos.x = 0;
                Player::Pos.y = 0;
            }
            if (!Mindfuck) {
                string bgm(Current["info"]["bgm"]);
                if (islower(bgm[0])) bgm[0] = toupper(bgm[0]);
                while (bgm.find(' ') != bgm.npos) bgm.erase(bgm.find(' '), 1);
                size_t p(bgm.find('/'));
                Node sn(NXSound[bgm.substr(0, p) + ".img"][bgm.substr(p + 1)]);
                if (!sn) Log::Write("Failed to find bgm " + bgm + " for map " + name);
                BGM.LoadNode(sn);
                BGM.setLoop(true);
                BGM.play();
            }
            Foothold::Load();
            View::Reset();
            Layer::LoadAll();
            Background::Load();
            Sprite::Cleanup();
        }
        void Render() {
            if (Mindfuck) {
                uniform_real_distribution<double> dist(0, 2 * M_PI);
                double t = Time::TDelta * 2.0822;
                Engine.seed(floor(t) + 1);
                double d = dist(Engine);
                double r(sin(d)), g(sin(d + M_PI * 2 / 3)), b(sin(d + M_PI * 4 / 3));
                if (t * 0.5 - floor(t * 0.5) < 0.5) {
                    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
                    GLfloat c[] = {r * 2, g * 2, b * 2, 1};
                    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, c);
                    glColor4f(1, 1, 1, 1);
                } else {
                    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
                    glColor4f(r * 2, g * 2, b * 2, 1);
                }
            }
            for (auto && b : Backgrounds) b.Render();
            Layer::RenderAll();
            for (auto && b : Foregrounds) b.Render();
            glDisable(GL_COLOR_LOGIC_OP);
            View::DrawEdges();
        }
        void Next() {
            uniform_int_distribution<size_t> dist(0, Maps.size() - 1);
            Engine.seed(Time::TDelta * 1000 + high_resolution_clock::now().time_since_epoch().count());
            Load(Maps[dist(Engine)]);
        }
    }
}