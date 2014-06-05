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
double const tau{6.28318530717958647692528676655900576839433879875021};
namespace view {
int x{0}, y{0};
int width{0}, height{0};
double fx{0}, fy{0};
double rx{0}, ry{0};
int left{0}, right{0}, top{0}, bottom{0};
int leftin{0}, rightin{0}, topin{0}, bottomin{0};
int cleft{0}, cright{0}, ctop{0}, cbottom{0};
bool doside{false}, dotop{false}, dobottom{false};
int xmin{0}, xmax{0}, ymin{0}, ymax{0};
double tx{0}, ty{0};
float r{1}, g{1}, b{1};
int bottomoffset{250};
template <typename T>
void restrict(T &p_x, T &p_y) {
    p_x = std::max<T>(std::min<T>(p_x, rightin), leftin);
    p_y = std::max<T>(std::min<T>(p_y, bottomin), topin);
}
void update_inner() {
    leftin = left + width / 2;
    rightin = right - width / 2;
    if (leftin > rightin) { leftin = rightin = (leftin + rightin) / 2; }
    topin = top + (height - bottomoffset);
    bottomin = bottom - bottomoffset;
    if (topin > bottomin) { topin = bottomin = (topin + bottomin) / 2; }
}
void resize(int p_width, int p_height) {
    width = p_width;
    height = p_height;
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
    update_inner();
}
void reset() {
    auto info = map::current["info"];
    if (info["VRTop"]) {
        top = info["VRTop"];
        bottom = info["VRBottom"];
        left = info["VRLeft"];
        right = info["VRRight"];
    } else {
        left = std::numeric_limits<int>::max();
        right = std::numeric_limits<int>::min();
        top = std::numeric_limits<int>::max();
        bottom = std::numeric_limits<int>::min();
        for (auto &f : footholds) {
            if (!f.initialized) { continue; }
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
    }
    ctop = info["LBTop"];
    cbottom = info["LBBottom"];
    cleft = info["LBSide"];
    cright = info["LBSide"];
    doside = !!info["LBSide"];
    dobottom = !!info["LBBottom"];
    dotop = !!info["LBTop"];
    tx = player::ch.pos.x;
    ty = player::ch.pos.y;
    fx = tx;
    fy = ty;
    update_inner();
    update();
}
void update() {
    tx = player::ch.pos.x;
    ty = player::ch.pos.y;
    auto dx = tx - fx;
    auto dy = ty - fy;
    auto adx = std::max(std::abs(dx) - 28, 0.);
    auto ady = std::max(std::abs(dy) - 28, 0.);
    auto sx = adx == 0 ? 0 : std::copysign(std::pow(adx, 1.5) + 28, dx) * time::delta * 0.2;
    auto sy = ady == 0 ? 0 : std::copysign(std::pow(ady, 1.5) + 28, dy) * time::delta * 0.2;
    if (std::abs(sx) > std::abs(tx - fx)) { sx = tx - fx; }
    if (std::abs(sy) > std::abs(ty - fy)) { sy = ty - fy; }
    fx += sx;
    fy += sy;
    restrict(fx, fy);
    if (config::rave) {
        std::random_device engine;
        auto num = std::pow(std::sin(time::delta_total * 2.088 * tau) * 0.5 + 0.5, 4) * 8;
        std::uniform_real_distribution<double> dist{-num, num};
        rx += dist(engine);
        ry += dist(engine);
        auto d = std::floor(time::delta_total * 2.088) * 1.95;
        r = static_cast<float>(std::sin(d));
        g = static_cast<float>(std::sin(d + 1 / 3. * tau));
        b = static_cast<float>(std::sin(d + 2 / 3. * tau));
        ::glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
        ::GLfloat c[4]{1 - r, 1 - g, 1 - b, 1};
        ::glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, c);
    } else {
        ::glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        r = 1, g = 1, b = 1;
    }
    rx *= 0.90;
    ry *= 0.90;
    x = static_cast<int>(std::round(fx + rx));
    y = static_cast<int>(std::round(fy + ry));
    xmin = x - width / 2;
    xmax = x + width / 2;
    ymin = y - (height - bottomoffset);
    ymax = y + bottomoffset;
    ::glMatrixMode(GL_PROJECTION);
    ::glLoadIdentity();
    ::glOrtho(0, width, height, 0, -1, 1);
    ::glMatrixMode(GL_MODELVIEW);
    ::glLoadIdentity();
}
void draw_edges() {
    /*sprite::unbind();
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
    glEnd();*/
}
}
}
