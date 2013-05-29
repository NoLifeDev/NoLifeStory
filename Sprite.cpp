//////////////////////////////////////////////////////////////////////////////
// NoLifeClient - Part of the NoLifeStory project                           //
// Copyright (C) 2013 Peter Atashian                                        //
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
#include "NoLifeClient.hpp"
namespace NL {
    unordered_map<size_t, GLuint> Sprites;
    GLuint LastBound(0);
    void BindTexture(Bitmap b) {
        GLuint t = Sprites[b.ID()];
        if (t) {
            if (t != LastBound) {
                LastBound = t;
                glBindTexture(GL_TEXTURE_2D, t);
            }
            return;
        }
        glGenTextures(1, &t);
        glBindTexture(GL_TEXTURE_2D, t);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, b.Width(), b.Height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, b.Data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        Sprites[b.ID()] = t;
    }
    Sprite::Sprite() : frame(0), delay(0), data() {}
    Sprite::Sprite(Sprite const & o) : frame(o.frame), delay(o.delay), data(o.data), repeat(o.repeat),
    movetype(o.movetype), movew(o.movew), moveh(o.moveh), movep(o.movep), mover(o.mover) {}
    Sprite::Sprite(Node const &o) : frame(0), delay(0), data(o) {
        Node n = data["0"] ? data["0"] : data;
        movetype = n["moveType"];
        movew = n["moveW"];
        moveh = n["moveH"];
        movep = n["moveP"];
        mover = n["moveR"];
        repeat = n["repeat"] ? data["repeat"].GetInt() : true;
    }
    Sprite & Sprite::operator=(Sprite const & o) {
        frame = o.frame;
        delay = o.delay;
        data = o.data;
        repeat = o.repeat;
        movetype = o.movetype;
        movew = o.movew;
        moveh = o.moveh;
        movep = o.movep;
        mover = o.mover;
        return *this;
    }
    void Sprite::Draw(int32_t x, int32_t y, bool view, bool flipped) {
        Node n;
        if (data["0"]) {
            delay += Time::Delta;
            n = data[frame];
            int32_t d = n["delay"];
            if (!d) d = 100;
            if (delay >= d) {
                delay -= d;
                if (!(n = data[++frame])) n = data[frame = 0];
            }
            if (n["a0"] || n["a1"]) {
                float a0 = n["a0"], a1 = n["a1"];
                glColor4ub(255, 255, 255, a0 - (a0 - a1) * delay / d);
            }
        } else n = data;
        if (n.T() != n.bitmap) return;
        Bitmap b(n);
        Node o(n["origin"]);
        x -= o.X(), y -= o.Y();
        uint16_t w(b.Width()), h(b.Height());
        if (view) {
            x += View::Width / 2, y += View::Height / 2;
            x -= View::X, y -= View::Y;
        }
        BindTexture(b);
        glBegin(GL_QUADS);
        flipped ? glTexCoord2i(1, 0) : glTexCoord2i(0, 0);
        glVertex2i(x, y);
        flipped ? glTexCoord2i(0, 0) : glTexCoord2i(1, 0);
        glVertex2i(x + w, y);
        flipped ? glTexCoord2i(0, 1) : glTexCoord2i(1, 1);
        glVertex2i(x + w, y + h);
        flipped ? glTexCoord2i(1, 1) : glTexCoord2i(0, 1);
        glVertex2i(x, y + h);
        glEnd();
        glColor4f(1, 1, 1, 1);
    }
}