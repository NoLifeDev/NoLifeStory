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

#include "foothold.hpp"
#include "map.hpp"
#include "sprite.hpp"
#include "view.hpp"
#include <algorithm>
#include <string>
#include <GL/glew.h>

namespace nl {
std::vector<foothold> footholds;
foothold::foothold(node n, int id, int group, int layer) : id{id}, group{group}, layer{layer} {
    x1 = n["x1"];
    x2 = n["x2"];
    y1 = n["y1"];
    y2 = n["y2"];
    force = n["force"];
    piece = n["piece"];
    nextid = n["next"];
    previd = n["prev"];
    cant_through = n["cantThrough"].get_bool();
    forbid_fall_down = n["forbidFallDown"].get_bool();
}
void foothold::load() {
    footholds.clear();
    for (auto layern : map::current["foothold"]) {
        auto layeri = std::stoi(layern.name());
        for (auto groupn : layern) {
            auto groupi = std::stoi(groupn.name());
            for (auto idn : groupn) {
                auto idi = std::stoi(idn.name());
                footholds.emplace_back(idn, idi, groupi, layeri);
            }
        }
    }
    std::sort(footholds.begin(), footholds.end(),
              [](foothold const &p_1, foothold const &p_2) { return p_1.id < p_2.id; });
    for (auto &fh : footholds) {
        auto pred = [](foothold const &p_fh, int p_id) { return p_fh.id < p_id; };
        auto nextit = std::lower_bound(footholds.cbegin(), footholds.cend(), fh.nextid, pred);
        fh.next = nextit != footholds.cend() && nextit->id == fh.nextid ? &*nextit : nullptr;
        auto previt = std::lower_bound(footholds.cbegin(), footholds.cend(), fh.previd, pred);
        fh.prev = previt != footholds.cend() && previt->id == fh.previd ? &*previt : nullptr;
    }
}
void foothold::draw_lines() {
    sprite::flush();
    glLoadIdentity();
    glLineWidth(3);
    glColor4f(0, 0, 0, 1);
    glBegin(GL_LINES);
    for (auto &fh : footholds) {
        glVertex2i(fh.x1 - view::xmin, fh.y1 - view::ymin);
        glVertex2i(fh.x2 - view::xmin, fh.y2 - view::ymin);
    }
    glEnd();
    glLineWidth(1);
    glColor4f(1, 0, 1, 1);
    glBegin(GL_LINES);
    for (auto &fh : footholds) {
        glVertex2i(fh.x1 - view::xmin, fh.y1 - view::ymin);
        glVertex2i(fh.x2 - view::xmin, fh.y2 - view::ymin);
    }
    glEnd();
}
}
