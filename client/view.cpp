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
        }
        void reset() {
            if (map::current["info"]["VRtop"]) {
                top = map::current["info"]["VRtop"];
                bottom = map::current["info"]["VRbottom"];
                left = map::current["info"]["VRleft"];
                right = map::current["info"]["VRright"];
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
                    if (left > f.x1)
                        left = f.x1;
                    if (left > f.x2)
                        left = f.x2;
                    if (right < f.x1)
                        right = f.x1;
                    if (right < f.x2)
                        right = f.x2;
                    if (top > f.y1)
                        top = f.y1;
                    if (top > f.y2)
                        top = f.y2;
                    if (bottom < f.y1)
                        bottom = f.y1;
                    if (bottom < f.y2)
                        bottom = f.y2;
                }
                top -= 100;
                bottom += 100;
                if (top > bottom - 600)
                    top = bottom - 600;
            }
            fx = 0;
            fy = 0;
            tx = 0;
            ty = 0;
            update();
        }
        void update() {
            auto sx = (tx - fx) * time::delta * 5;
            auto sy = (ty - fy) * time::delta * 5;
            if (abs(sx) > abs(tx - fx))
                sx = tx - fx;
            if (abs(sy) > abs(ty - fy))
                sy = ty - fy;
            fx += sx;
            fy += sy;
            restrict(fx, fy);
            if (config::rave) {
                std::random_device engine;
                auto n1 = {-4, -2, 0, 2, 4}, n2 = {0, 1, 8, 1, 0};
                std::piecewise_linear_distribution<double> dist(n1.begin(), n1.end(), n2.begin());
                rx += dist(engine);
                ry += dist(engine);
            }
            rx *= 0.95;
            ry *= 0.95;
            x = static_cast<int>(fx + rx);
            y = static_cast<int>(fy + ry);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            if (config::rave) {
                gluPerspective(-5 * std::pow(0.5 * std::sin(time::delta_total * 2.088 * 2 * pi) + 0.5, 9) + 90, static_cast<double>(width) / height, 0.1, 10000);
                gluLookAt(width / 2, height / 2, 0 - height / 2, width / 2, height / 2, 0, 0, -1, 0);
                auto d = floor(time::delta_total * 2.088 - 0.1) * 1.95;
                auto r = sin(d), g = sin(d + 2 / 3. * pi), b = sin(d + 4 / 3. * pi);
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
                GLfloat c[] = {static_cast<GLfloat>(r), static_cast<GLfloat>(g), static_cast<GLfloat>(b), 1};
                glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, c);
                glColor4d(1 - r, 1 - g, 1 - b, 1);
            } else {
                glOrtho(0, width, height, 0, -1, 1);
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            }
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            xmin = x - width / 2;
            xmax = x + width / 2;
            ymin = y - height / 2;
            ymax = y + height / 2;
        }
        void draw_edges() {
            sprite::unbind();
            glColor4d(0, 0, 0, 1);
            glBegin(GL_QUADS);
            glVertex2i(0, 0);
            glVertex2i(right - xmin, 0);
            glVertex2i(right - xmin, top - ymin);
            glVertex2i(0, top - ymin);
            glVertex2i(right - xmin, 0);
            glVertex2i(width, 0);
            glVertex2i(width, bottom - ymin);
            glVertex2i(right - xmin, bottom - ymin);
            glVertex2i(left - xmin, bottom - ymin);
            glVertex2i(width, bottom - ymin);
            glVertex2i(width, height);
            glVertex2i(left - xmin, height);
            glVertex2i(0, top - ymin);
            glVertex2i(left - xmin, top - ymin);
            glVertex2i(left - xmin, height);
            glVertex2i(0, height);
            glEnd();
        }
    }
}
