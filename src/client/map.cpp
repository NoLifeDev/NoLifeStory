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
#include "portal.hpp"
#include "sound.hpp"
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
        std::string current_name;
        std::vector<std::pair<std::string, unsigned>> results;
        bool old_style;
        void load(std::string name, std::string port) {
            if (name.size() < 9)
                name.insert(0, 9 - name.size(), '0');
            auto m = old_style ? map_node[name + ".img"] :
                map_node[std::string("Map") + name[0]][name + ".img"];
            //If the map is invalid just ignore it
            if (!m)
                return;
            //Some maps link to other maps. I have no idea why.
            if (m["info"]["link"])
                return load(m["info"]["link"], port);
            next = m;
        }
        void add_random(node n) {
            auto name = n.name();
            //Ignore anything which isn't obviously a map
            if (name.size() != 13)
                return;
            all_maps.emplace_back(name.substr(0, name.size() - 4));
        }
        void init_random() {
            if (old_style)
            for (auto n : map_node)
                add_random(n);
            else
            for (auto i = 0u; i <= 9; ++i)
            for (auto n : map_node["Map" + std::to_string(i)])
                add_random(n);
        }
        void load_random() {
            std::random_device rand;
            std::uniform_int_distribution<size_t> dist(0, all_maps.size() - 1);
            load(all_maps[dist(rand)], "sp");
        }
        void load_now() {
            current = next;
            current_name = current.name();
            current_name.erase(current_name.find(".img"));
            std::cout << "Loading map " << current.name() << std::endl;
            time::reset();
            music::play();
            sprite::cleanup();
            layer::load();
            background::load();
            foothold::load();
            portal::load();
            view::reset();
        }
        void init() {
            map_node = nx::map["Map"];
            old_style = !map_node["Map0"];
            init_random();
            load_random();
            load_now();
        }
        void update() {
            if (next != current)
                load_now();
        }
        void render() {
            for (auto & b : backgrounds)
                b.render();
            layer::render();
            for (auto & p : portals)
                p.render();
            for (auto & b : foregrounds)
                b.render();
            view::draw_edges();
        }
    }
}
