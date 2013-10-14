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

#include "view.hpp"
#include "config.hpp"
#include "map.hpp"
#include <GL/glew.h>
#include <algorithm>

namespace nl {
    namespace view {
        int32_t x {0}, y {0};
        int32_t width {0}, height {0};
        double fx {0}, fy {0};
        int32_t left {0}, right {0}, top {0}, bottom {0};
        template <typename T>
        void restrict(T & x, T & y) {
            if (right - left <= width) x = (right + left) / 2;
            else x = std::max(std::min(x, right - width / 2), left + width / 2);
            if (bottom - top <= height) y = (bottom + top) / 2;
            else y = std::max(std::min(x, bottom - height / 2), top + height / 2);
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
        void Reset() {
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
                left = std::numeric_limits<int32_t>::max(), right = std::numeric_limits<int32_t>::min();
                top = std::numeric_limits<int32_t>::max(), bottom = std::numeric_limits<int32_t>::min();
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
            //X = Player::Pos.x, Y = Player::Pos.y;
            restrict(x, y);
            fx = x;
            fy = y;
        }
        void Update() {
            double tx(Player::Pos.x), ty(Player::Pos.y);
            Restrict(tx, ty);
            double sx((tx - FX) * Time::Delta * 5), sy((ty - FY) * Time::Delta * 5);
            if (abs(sx) > abs(tx - FX)) sx = tx - FX;
            if (abs(sy) > abs(ty - FY)) sy = ty - FY;
            FX += sx, FY += sy;
            Restrict(FX, FY);
            X = FX, Y = FY;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            if (Config::Rave) {
                mt19937_64 engine(high_resolution_clock::now().time_since_epoch().count());
                uniform_int_distribution<int> dist(-10, 10);
                X += dist(engine);
                Y += dist(engine);
                gluPerspective(-10 * pow(0.5 * sin(Time::TDelta * 2.088 * 2 * M_PI) + 0.5, 9) + 90, double(width) / height, 0.1, 10000);
                gluLookAt(width / 2, height / 2, -height / 2, width / 2, height / 2, 0, 0, -1, 0);
            } else glOrtho(0, width, height, 0, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
        void DrawEdges() {
            Sprite::Unbind();
            glColor4f(0, 0, 0, 1);
            Graphics::DrawRect(0, 0, width, top - Y + height / 2, false);
            Graphics::DrawRect(0, bottom - Y + height / 2, width, height, false);
            Graphics::DrawRect(0, top - Y + height / 2, left - X + width / 2, bottom - Y + height / 2, false);
            Graphics::DrawRect(right - X + width / 2, top - Y + height / 2, width, bottom - Y + height / 2, false);
        }
    }
}