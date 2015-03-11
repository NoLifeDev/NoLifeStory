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

#include "character.hpp"
#include "sprite.hpp"
#include "time.hpp"
#include <nx/nx.hpp>
#include <algorithm>
#include <map>

namespace nl {
character::part::part(unsigned p_id) { set(p_id); }
void character::part::set(unsigned p_id) {
    m_id = p_id;
    auto category = m_id / 10000;
    auto specific = m_id % 10000;
    auto prefix = std::to_string(category);
    prefix.insert(0, 4 - prefix.length(), '0');
    auto part = std::to_string(specific);
    part.insert(0, 4 - part.length(), '0');
    auto name = prefix + part + ".img";
    switch (category) {
    case 0:
    case 1: m_node = nx::character[name]; break;
    case 2: m_node = nx::character["Face"][name]; break;
    case 3: m_node = nx::character["Hair"][name]; break;
    case 100: m_node = nx::character["Cap"][name]; break;
    case 104: m_node = nx::character["Coat"][name]; break;
    case 105: m_node = nx::character["Longcoat"][name]; break;
    case 106: m_node = nx::character["Pants"][name]; break;
    case 107: m_node = nx::character["Shoes"][name]; break;
    case 108: m_node = nx::character["Glove"][name]; break;
    case 109: m_node = nx::character["Shield"][name]; break;
    case 110: m_node = nx::character["Cape"][name]; break;
    case 101:
    case 102:
    case 103:
    case 112:
    case 113:
    case 114:
    case 115:
    case 116:
    case 118:
    case 119: m_node = nx::character["Accessory"][name]; break;
    default: throw std::runtime_error{"Unknown part category"};
    }
}
void character::render() {
    if (pos.right && !pos.left) { flipped = true; } else if (pos.left && !pos.right) {
        flipped = false;
    }
    auto set_state = [this](std::string p_state) {
        if (state != p_state) {
            state = p_state;
            frame = 0;
            delay = 0;
        }
    };
    if (!pos.fh) { set_state("jump"); } else if (pos.left ^ pos.right) {
        set_state("walk1");
    } else { set_state("stand1"); }
    struct sub_part {
        node m_node;
        int x, y, z;
        bool done;
    };
    struct mapping {
        int x, y;
    };
    std::map<std::string, mapping> mappings;
    std::vector<sub_part> sub_parts;
    auto anim = nx::character["00002000.img"][state][frame];
    auto d = anim["delay"].get_real(100);
    delay += time::delta * 1000;
    if (delay >= d) {
        delay -= d;
        ++frame;
        if (!nx::character["00002000.img"][state][frame]) { frame = 0; }
    }
    auto zmap = nx::base["zmap.img"];
    for (auto & p : m_parts) {
        auto n = p.m_node[state][frame];
        auto s0 = p.m_node.name();
        auto s1 = n.name();
        for (auto nn : n) {
            auto s2 = nn.name();
            if (nn["map"]) {
                sub_parts.emplace_back();
                sub_parts.back().m_node = nn;
                sub_parts.back().done = false;
                sub_parts.back().z = zmap[nn.name()];
            } else {
                // What was I trying to do here?
                auto str = nn.name();
            }
        }
    }
    bool done = false;
    while (!done) {
        done = true;
        for (auto & p : sub_parts) {
            if (!p.done) {
                if (p.m_node.name() == "body") {
                    p.x = static_cast<int>(pos.x);
                    p.y = static_cast<int>(pos.y);
                    p.done = true;
                } else
                    for (auto n : p.m_node["map"]) {
                        auto it = mappings.find(n.name());
                        if (it != mappings.end()) {
                            if (flipped) { p.x = it->second.x + n.x(); } else {
                                p.x = it->second.x - n.x();
                            }
                            p.y = it->second.y - n.y();
                            p.done = true;
                        }
                    }
                if (p.done) {
                    for (auto n : p.m_node["map"]) {
                        auto it = mappings.find(n.name());
                        if (it == mappings.end()) {
                            mapping m;
                            if (flipped) { m.x = p.x - n.x(); } else {
                                m.x = p.x + n.x();
                            }
                            m.y = p.y + n.y();
                            mappings.insert(std::make_pair(n.name(), m));
                        }
                    }
                } else { done = false; }
            }
        }
    }
    std::sort(sub_parts.begin(), sub_parts.end(),
              [](sub_part const & a, sub_part const & b) { return a.z > b.z; });
    auto flags = sprite::relative;
    if (flipped) { flags |= sprite::flipped; }
    for (auto & p : sub_parts) {
        sprite spr = p.m_node;
        spr.draw(p.x, p.y, flags);
    }
}
void character::update() { pos.update(); }
}
