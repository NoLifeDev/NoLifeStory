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
#include "time.hpp"
#include "foothold.hpp"
#include "sprite.hpp"
#include "player.hpp"
#include <GL/glew.h>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>
#include <iostream>

namespace nl {
    double const pi = 3.14159265358979323846264338327950288419716939937510582;
    namespace view {
        int x = 0, y = 0;
        int width = 0, height = 0;
        double fx = 0, fy = 0;
        double rx = 0, ry = 0;
        int left = 0, right = 0, top = 0, bottom = 0;
        int cleft = 0, cright = 0, ctop = 0, cbottom = 0;
        bool doside = false, dotop = false, dobottom = false;
        int xmin = 0, xmax = 0, ymin = 0, ymax = 0;
        double tx = 0, ty = 0;
        template <typename T>
        void restrict(T & x, T & y) {
            if (right - left <= width)
            x = (right + left) / 2;
            else
                x = std::max<T>(std::min<T>(x, right - width / 2), left + width / 2);
            if (bottom - top <= height)
                y = (bottom + top) / 2;
            else
                y = std::max<T>(std::min<T>(y, bottom - height / 2), top + height / 2);
        }
        void resize(int w, int h) {
            width = w;
            height = h;
            glViewport(0, 0, width, height);
            if (config::fullscreen) {
                config::fullscreen_width = width;
                config::fullscreen_height = height;
            } else {
                config::window_width = width;
                config::window_height = height;
            }
            if (config::stretch) {
                width = 800;
                height = 600;
            }
        }
        void reset() {
            auto info = map::current["info"];
            if (info["VRTop"]) {
                top = info["VRTop"];
                bottom = info["VRBottom"];
                left = info["VRLeft"];
                right = info["VRRight"];
                if (bottom - top < 600) {
                    auto d = (600 - bottom + top) / 2;
                    bottom += d;
                    top -= d;
                }
                if (right - left < 800) {
                    auto d = (800 - right + left) / 2;
                    right += d;
                    left -= d;
                }
            } else {
                left = std::numeric_limits<int>::max();
                right = std::numeric_limits<int>::min();
                top = std::numeric_limits<int>::max();
                bottom = std::numeric_limits<int>::min();
                for (foothold & f : footholds) {
                    if (!f.initialized)
                        continue;
                    left = std::min(left, f.x1);
                    left = std::min(left, f.x2);
                    right = std::max(right, f.x1);
                    right = std::max(right, f.x2);
                    top = std::min(top, f.y1);
                    top = std::min(top, f.y2);
                    bottom = std::max(bottom, f.y1);
                    bottom = std::max(bottom, f.y2);
                }
                top -= 256;
                bottom += 64;
                if (top > bottom - 600)
                    top = bottom - 600;
            }
            ctop = info["LBTop"];
            cbottom = info["LBBottom"];
            cleft = info["LBSide"];
            cright = info["LBSide"];
            doside = !!info["LBSide"];
            dobottom = !!info["LBBottom"];
            dotop = !!info["LBTop"];
            tx = player::pos.x;
            ty = player::pos.y;
            fx = tx;
            fy = ty;
            update();
        }
        void update() {
            tx = player::pos.x;
            ty = player::pos.y;
            auto dx = tx - fx;
            auto dy = ty - fy;
            dx = std::copysign(std::max(std::abs(dx) - 30, 0.), dx);
            dy = std::copysign(std::max(std::abs(dy) - 30, 0.), dy);
            auto sx = dx * time::delta * 3;
            auto sy = dy * time::delta * 3;
            if (abs(sx) > abs(tx - fx))
                sx = tx - fx;
            if (abs(sy) > abs(ty - fy))
                sy = ty - fy;
            fx += sx;
            fy += sy;
            restrict(fx, fy);
            if (config::rave) {
                std::random_device engine;
                auto num = std::pow(std::sin(time::delta_total * 2.088 * 2 * pi) * 0.5 + 0.5, 4) * 8;
                std::uniform_real_distribution<double> dist(-num, num);
                rx += dist(engine);
                ry += dist(engine);
                auto d = floor(time::delta_total * 2.088 - 0.1) * 1.95;
                auto r = sin(d), g = sin(d + 2 / 3. * pi), b = sin(d + 4 / 3. * pi);
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
                GLfloat c[] = {static_cast<GLfloat>(r), static_cast<GLfloat>(g), static_cast<GLfloat>(b), 1};
                glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, c);
                glColor4d(1 - r, 1 - g, 1 - b, 1);
            } else {
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            }
            rx *= 0.90;
            ry *= 0.90;
            x = static_cast<int>(fx + rx);
            y = static_cast<int>(fy + ry);
            xmin = x - width / 2;
            xmax = x + width / 2;
            ymin = y - height / 2;
            ymax = y + height / 2;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, width, height, 0, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
        void draw_edges() {
            sprite::unbind();
            auto doclip = doside || dotop || dobottom;
            auto xmid = (right + left) / 2 - xmin;
            auto ymid = (top + bottom) / 2 - ymin;
            auto xb = std::max(0, (right - left - 1366) / 2);
            auto yb = std::max(0, (bottom - top - 768) / 2);
            auto nleft = doside ? xmid - 512 + cleft - xb : !doclip ? left - xmin : 0;
            auto nright = doside ? xmid + 512 - cright + xb : !doclip ? right - xmin : width;
            auto ntop = dotop ? ymid - 384 + ctop - yb: !doclip ? top - ymin : 0;
            auto nbottom = dobottom ? ymid + 319 - cbottom + yb: !doclip ? bottom - ymin : height;
            glColor4d(0, 0, 0, 1);
            glBegin(GL_QUADS);
            glVertex2i(0, 0);
            glVertex2i(width, 0);
            glVertex2i(width, ntop);
            glVertex2i(0, ntop);
            glVertex2i(0, height);
            glVertex2i(width, height);
            glVertex2i(width, nbottom);
            glVertex2i(0, nbottom);
            glVertex2i(0, 0);
            glVertex2i(0, height);
            glVertex2i(nleft, height);
            glVertex2i(nleft, 0);
            glVertex2i(width, 0);
            glVertex2i(width, height);
            glVertex2i(nright, height);
            glVertex2i(nright, 0);
            glEnd();
        }
    }
}
