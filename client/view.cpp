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

#define _USE_MATH_DEFINES
#include "view.hpp"
#include "config.hpp"
#include "map.hpp"
#include "time.hpp"
#include <GL/glew.h>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>

namespace nl {
    namespace view {
        int32_t x {0}, y {0};
        int32_t width {0}, height {0};
        double fx {0}, fy {0};
        int32_t left {0}, right {0}, top {0}, bottom {0};
        template <typename T>
        void restrict(T & x, T & y) {
            if (right - left <= width) x = (right + left) / 2;
            else x = std::max<int32_t>(std::min<int32_t>(x, right - width / 2), left + width / 2);
            if (bottom - top <= height) y = (bottom + top) / 2;
            else y = std::max<int32_t>(std::min<int32_t>(x, bottom - height / 2), top + height / 2);
        }
        void resize(int32_t w, int32_t h) {
            width = w, height = h;
            glViewport(0, 0, width, height);
            if (config::fullscreen) {
                config::fullscreen_width = width;
                config::fullscreen_height = height;
            } else {
                config::window_width = width;
                config::window_height = height;
            }
        }
        void reset() {
            if (map::current["info"]["VRtop"]) {
                top = map::current["info"]["VRtop"];
                bottom = map::current["info"]["VRbottom"];
                left = map::current["info"]["VRleft"];
                right = map::current["info"]["VRright"];
                if (bottom - top < 600) {
                    int32_t d = (600 - bottom + top) / 2;
                    bottom += d;
                    top -= d;
                }
                if (right - left < 800) {
                    int32_t d = (800 - right + left) / 2;
                    right += d;
                    left -= d;
                }
            } else {
                left = std::numeric_limits<int32_t>::max();
                right = std::numeric_limits<int32_t>::min();
                top = std::numeric_limits<int32_t>::max();
                bottom = std::numeric_limits<int32_t>::min();
                //for (auto & f : footholds) {
                //    if (left > f.x1) left = f.x1;
                //    if (left > f.x2) left = f.x2;
                //    if (right < f.x1) right = f.x1;
                //    if (right < f.x2) right = f.x2;
                //    if (top > f.y1) top = f.y1;
                //    if (top > f.y2) top = f.y2;
                //    if (bottom < f.y1) bottom = f.y1;
                //    if (bottom < f.y2) bottom = f.y2;
                //}
                top -= 256;
                bottom += 128;
                if (top > bottom - 600) top = bottom - 600;
            }
            x = 0;
            y = 0;
            //X = Player::Pos.x, Y = Player::Pos.y;
            restrict(x, y);
            fx = x;
            fy = y;
        }
        void update() {
            double tx {0};
            double ty {0};
            //double tx(Player::Pos.x), ty(Player::Pos.y);
            restrict(tx, ty);
            double sx {(tx - fx) * time::delta * 5};
            double sy {(ty - fy) * time::delta * 5};
            if (abs(sx) > abs(tx - fx)) sx = tx - fx;
            if (abs(sy) > abs(ty - fy)) sy = ty - fy;
            fx += sx;
            fy += sy;
            restrict(fx, fy);
            x = fx;
            y = fy;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            if (config::rave) {
                std::mt19937_64 engine {static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())};
                std::uniform_int_distribution<int32_t> dist {-10, 10};
                x += dist(engine);
                y += dist(engine);
                gluPerspective(-10 * std::pow(0.5 * std::sin(time::delta_total * 2.088 * 2 * M_PI) + 0.5, 9) + 90, static_cast<double>(width) / height, 0.1, 10000);
                gluLookAt(width / 2, height / 2, -height / 2, width / 2, height / 2, 0, 0, -1, 0);
            } else glOrtho(0, width, height, 0, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
        void draw_edges() {
            //Sprite::Unbind();
            //glColor4f(0, 0, 0, 1);
            //Graphics::DrawRect(0, 0, width, top - Y + height / 2, false);
            //Graphics::DrawRect(0, bottom - Y + height / 2, width, height, false);
            //Graphics::DrawRect(0, top - Y + height / 2, left - X + width / 2, bottom - Y + height / 2, false);
            //Graphics::DrawRect(right - X + width / 2, top - Y + height / 2, width, bottom - Y + height / 2, false);
        }
    }
}
