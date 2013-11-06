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

#include "sprite.hpp"
#include "config.hpp"
#include "time.hpp"
#include "view.hpp"
#include <nx/bitmap.hpp>
#include <GL/glew.h>
#include <unordered_map>
#include <deque>

namespace nl {
    namespace {
        std::unordered_map<size_t, GLuint> textures;
        std::deque<size_t> loaded_textures;
        size_t last_bound = 0;
        GLuint vbo = 0;
    }
    void sprite::init() {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        float a[] = {0, 0, 1, 0, 1, 1, 0, 1};
        glBufferData(GL_ARRAY_BUFFER, sizeof(a), a, GL_STATIC_DRAW);
    }
    void sprite::reinit() {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, nullptr);
        glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
    }
    void sprite::bind() {
        bitmap b = current;
        if (b.id() == last_bound) return;
        if (!last_bound) reinit();
        last_bound = b.id();
        GLuint & t = textures[b.id()];
        if (!t) {
            glGenTextures(1, &t);
            glBindTexture(GL_TEXTURE_2D, t);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, b.width(), b.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, b.data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            loaded_textures.push_back(b.id());
        } else {
            glBindTexture(GL_TEXTURE_2D, t);
        }
    }
    void sprite::unbind() {
        last_bound = 0;
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
    void sprite::cleanup() {
        while (static_cast<int>(loaded_textures.size()) > config::max_textures) {
            size_t s = loaded_textures.front();
            loaded_textures.pop_front();
            auto it = textures.find(s);
            glDeleteTextures(1, &it->second);
            textures.erase(it);
        }
    }
    sprite::sprite() {}
    sprite::sprite(node o) : data(o) {
        if (data.data_type() == node::type::bitmap) {
            animated = false;
        } else if (data["0"].data_type() == node::type::bitmap) {
            animated = true;
        } else {
            data = {};
        }
        set_frame(0);
    }
    void sprite::set_frame(int f) {
        if (!data) return;
        frame = f;
        if (!animated) {
            current = data;
        } else {
            current = data[frame];
            if (!current) {
                current = data[frame = 0];
            }
            delay = 0;
            next_delay = current["delay"].get_real(100);
        }
        bitmap b = current;
        width = b.width();
        height = b.height();
        node o = current["origin"];
        originx = o.x();
        originy = o.y();
        movetype = current["moveType"];
        if (movetype) {
            movew = current["moveW"];
            moveh = current["moveH"];
            movep = current["moveP"];
            mover = current["moveR"];
        }
        repeat = current["repeat"].get_bool();
        if (current["a0"] || current["a1"]) {
            a0 = current["a0"].get_real(0) / 255.;
            a1 = current["a1"].get_real(0) / 255.;
        } else {
            a0 = 1;
            a1 = 1;
        }
    }
    void sprite::draw(int x, int y, flags f, int cx, int cy) {
        if (!data) return;
        if (animated) {
            delay += time::delta * 1000;
            if (delay > next_delay) {
                set_frame(frame + 1);
            }
        }
        if (!cx) cx = width;
        else if (cx < 0) cx = -cx;
        if (!cy) cy = height;
        else if (cy < 0) cy = -cy;
        f & flipped ? x -= width - originx : x -= originx;
        y -= originy;
        if (f & relative) {
            x -= view::xmin;
            y -= view::ymin;
        }
        if (x + width < 0) return;
        if (x > view::width) return;
        if (y + height < 0) return;
        if (y > view::height) return;
        if (animated) {
            double dif = delay / next_delay;
            glColor4f(1, 1, 1, dif * a1 + (1 - dif) * a0);
        } else {
            glColor4f(1, 1, 1, 1);
        }
        double angle = 0;
        bind();
        auto single = [&]() {
            glLoadIdentity();
            glTranslated(x + originx, y + originy, 0);
            if (angle) glRotated(angle, 0, 0, 1);
            glTranslated(f & flipped ? width - originx : -originx, -originy, 0);
            glScaled(f & flipped ? -width : width, height, 1);
            glDrawArrays(GL_QUADS, 0, 4);
        };
        single();
        /*float alpha {1};
        if (data != next) {
            delay += Time::Delta * 1000;
            int32_t d(next["delay"]);
            if (!d) d = 100;
            if (delay >= d) {
                delay = 0;
                if (!(next = data[++frame])) next = data[frame = 0];
            }
            if (next["a0"] || next["a1"]) {
                double a0(next["a0"]), a1(next["a1"]);
                double dif(double(delay) / d);
                alpha = (a0 * (1 - dif) + a1 * dif) / 255;
            }
        }
        Bitmap b;
        if (next.T() == Node::Type::Bitmap && BindTexture(b = next)) last = next;
        else if (last.T() == Node::Type::Bitmap && BindTexture(b = last));
        else return;
        Node o(last["origin"]);
        uint16_t w(b.Width()), h(b.Height());
        int32_t ox(o.X()), oy(o.Y());
        flipped ? x -= w - ox : x -= ox, y -= oy;
        if (view) {
            x += View::Width / 2, y += View::Height / 2;
            x -= View::X, y -= View::Y;
        }
        double ang(0);
        switch (movetype) {
        case 1:
            if (movep) x += movew * sin(Time::TDelta * 1000 * 2 * M_PI / movep);
            else x += movew * sin(Time::TDelta);
            break;
        case 2:
            if (movep) y += moveh * sin(Time::TDelta * 1000 * 2 * M_PI / movep);
            else y += moveh * sin(Time::TDelta);
            break;
        case 3:
            ang = Time::TDelta * 1000 * 180 / M_PI / mover;
            break;
        }
        if (!Config::Rave) glColor4f(1, 1, 1, alpha);
        auto single = [&]() {
            glTranslated(x + ox, y + oy, 0);
            glRotated(ang, 0, 0, 1);
            glTranslated(flipped ? w - ox : -ox, -oy, 0);
            glScaled(flipped ? -w : w, h, 1);
            glDrawArrays(GL_QUADS, 0, 4);
            glLoadIdentity();
        };
        if (tilex) {
            if (tiley) {
                if (!cx) cx = w;
                int32_t x1 = x % cx - cx;
                int32_t x2 = (x - View::Width) % cx + View::Width + cx;
                if (!cy) cy = h;
                int32_t y1 = y % cy - cy;
                int32_t y2 = (y - View::Height) % cy + View::Height + cy;
                if (cx == w && cy == h) {
                    glBegin(GL_QUADS);
                    glTexCoord2i(0, 0);
                    glVertex2i(x1, y1);
                    glTexCoord2i((x2 - x1) / cx, 0);
                    glVertex2i(x2, y1);
                    glTexCoord2i((x2 - x1) / cx, (y2 - y1) / cy);
                    glVertex2i(x2, y2);
                    glTexCoord2i(0, (y2 - y1) / cy);
                    glVertex2i(x1, y2);
                    glEnd();
                } else for (x = x1; x < x2; x += cx) for (y = y1; y < y2; y += cy) single();
            } else {
                if (!cx) cx = w;
                int32_t x1 = x % cx - cx;
                int32_t x2 = (x - View::Width) % cx + View::Width + cx;
                for (x = x1; x < x2; x += cx) single();
            }
        } else {
            if (tiley) {
                if (!cy) cy = h;
                int32_t y1 = y % cy - cy;
                int32_t y2 = (y - View::Height) % cy + View::Height + cy;
                for (y = y1; y < y2; y += cy) single();
            } else if (x + w > 0 && x < View::Width && y + h > 0 && y < View::Height) single();
        }*/
    }
    sprite::flags & operator|=(sprite::flags & a, sprite::flags b) {
        a = static_cast<sprite::flags>(static_cast<int>(a) | static_cast<int>(b));
        return a;
    }
    sprite::flags operator|(sprite::flags a, sprite::flags b) {
        return static_cast<sprite::flags>(static_cast<int>(a) | static_cast<int>(b));
    }
}
