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

#include "map.hpp"
#include "layer.hpp"
#include "view.hpp"
#include "foothold.hpp"
#include "background.hpp"
#include "time.hpp"
#include <nx/nx.hpp>
#include <nx/node.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <iostream>

namespace nl {
    namespace map {
        std::vector<std::string> all_maps;
        node map_node;
        node current, next;
        std::vector<std::pair<std::string, unsigned>> results;
        void load(std::string name, std::string portal) {
            if (name.size() < 9)
                name.insert(0, name.size(), '0');
            auto m = map_node[std::string("Map") + name[0]][name + ".img"];
            if (!m)
                return;
            if (m["info"]["link"]) {
                load(m["info"]["link"], portal);
                return;
            }
            next = m;
        }
        void load_random() {
            std::random_device rand;
            std::uniform_int_distribution<size_t> dist(0, all_maps.size());
            load(all_maps[dist(rand)], "sp");
        }
        void load_now() {
            current = next;
            std::cout << "Loading map " << current.name() << std::endl;
            time::reset();
            sprite::cleanup();
            layer::load();
            background::load();
            foothold::load();
            view::reset();
        }
        void init() {
            map_node = nx::map["Map"];
            for (auto i = 0u; i <= 9; ++i)
            for (auto n : map_node["Map" + std::to_string(i)]) {
                auto name = n.name();
                if (name.size() < 4)
                    continue;//This shouldn't happen
                all_maps.emplace_back(name.substr(0, name.size() - 4));
            }
            load_random();
            load_now();
        }
        void update() {
            if (next != current)
                load_now();
        }
        void render() {
            for (background & b : backgrounds)
                b.render();
            layer::render();
            for (background & b : foregrounds)
                b.render();
            view::draw_edges();
        }
        /*
        void Render() {
            for (auto && b : Backgrounds) b.Render();
            Layer::RenderAll();
            for (auto && p : NL::Portals) p.Render();
            for (auto && b : Foregrounds) b.Render();
            View::DrawEdges();
            if (Shade > 0) {
                glColor4f(0, 0, 0, pow(Shade, 2));
                Graphics::DrawRect(0, 0, View::Width, View::Height, false);
            }
            if (Shade < 0) Shade = 0;
            if (Next) {
                Shade += Time::Delta * 10;
                if (Shade > 1) {
                    Map::LoadNow();
                    Shade = 1.5;
                }
            } else if (Shade > 0) Shade -= Time::Delta * 5;
            Sprite::Unbind();
            double c = sin(Time::TDelta * 10) * 0.5 + 0.5;
            glColor4d(sin(Time::TDelta * 2 * M_PI) * 0.5 + 0.5, sin(Time::TDelta * 2 * M_PI + M_PI * 2 / 3) * 0.5 + 0.5, sin(Time::TDelta * 2 * M_PI + M_PI * 4 / 3) * 0.5 + 0.5, 1);
            glBegin(GL_LINES);
            for (Foothold & f : Footholds) {
                glVertex2d(f.x1 + View::Width / 2 - View::X, f.y1 + View::Height / 2 - View::Y);
                glVertex2d(f.x2 + View::Width / 2 - View::X, f.y2 + View::Height / 2 - View::Y);
            }
            glEnd();
        }
        */
    }
}
