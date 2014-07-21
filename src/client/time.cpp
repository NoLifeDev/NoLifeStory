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

#include "time.hpp"
#include "config.hpp"
#include "sprite.hpp"
#include <GL/glew.h>
#include <deque>
#include <chrono>
#include <thread>

namespace nl {
namespace time {
using clock = std::chrono::high_resolution_clock;
using seconds_double = std::chrono::duration<double>;
using namespace std::chrono_literals;
using std::chrono::duration_cast;
unsigned fps = 0;
double delta = 0, delta_total = 0;
clock::time_point first{clock::now()};
std::deque<clock::time_point> frames{first};
void reset() {
    first = clock::now();
    delta_total = 0;
}
void draw() {
    if (!config::debug) { return; }
    auto const xscale = 8;
    auto const yscale = 8192;
    sprite::flush();
    glLoadIdentity();
    glLineWidth(2);
    glColor4f(1, 0, 0, 1);
    glBegin(GL_LINE_STRIP);
    glVertex2i(0, yscale / config::target_fps);
    glVertex2i(static_cast<GLint>(frames.size() * xscale), yscale / config::target_fps);
    glEnd();
    glLineWidth(3);
    glColor4f(0, 0, 0, 1);
    glBegin(GL_LINE_STRIP);
    for (auto i = 1u; i < frames.size(); ++i) {
        auto t = duration_cast<seconds_double>(frames[i] - frames[i - 1]);
        glVertex2i(static_cast<GLint>(i * xscale), static_cast<GLint>(yscale * t.count()));
    }
    glEnd();
    glLineWidth(1);
    glColor4f(1, 1, 1, 1);
    glBegin(GL_LINE_STRIP);
    for (auto i = 1u; i < frames.size(); ++i) {
        auto t = duration_cast<seconds_double>(frames[i] - frames[i - 1]);
        glVertex2i(static_cast<GLint>(i * xscale), static_cast<GLint>(yscale * t.count()));
    }
    glEnd();
}
void update() {
    auto last = frames.back();
    if (config::target_fps <= 0) { config::target_fps = 1; } // Because some people are idiots
    auto step_size = duration_cast<clock::duration>(1s) / config::target_fps;
    auto pre = clock::now();
    if (config::limit_fps && pre - last + 1ms < step_size) {
        std::this_thread::sleep_for(step_size - (pre - last + 1ms));
    }
    auto now = clock::now();
    delta = duration_cast<seconds_double>(now - last).count();
    if (delta < 0) { delta = 0; }
    if (delta > 0.05) { delta = 0.05; }
    delta_total = duration_cast<seconds_double>(now - first).count();
    while (!frames.empty() && now - frames.front() > 1s) { frames.pop_front(); }
    frames.push_back(now);
    fps = static_cast<unsigned>(frames.size());
    draw();
}
}
}
