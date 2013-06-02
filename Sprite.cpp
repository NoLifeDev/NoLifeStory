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
    deque<size_t> LoadedSprites;
    GLuint LastBound(0);
    size_t const MaxTextures = 0x800;
    void Sprite::LoseBind() {
        LastBound = 0;
    }
    void Sprite::Cleanup() {
        while (LoadedSprites.size() > MaxTextures) {
            size_t s = LoadedSprites.front();
            LoadedSprites.pop_front();
            glDeleteTextures(1, &Sprites[s]);
            Sprites.erase(s);
        }
    }
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        Sprites[b.ID()] = t;
        LastBound = t;
        LoadedSprites.push_back(b.ID());
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
        repeat = n["repeat"].GetBool();
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
    void Sprite::Draw(int32_t x, int32_t y, bool view, bool flipped, bool tilex, bool tiley, int32_t cx, int32_t cy) {
        Node n;
        float alpha(1);
        if (data.T() != Node::bitmap) {
            delay += Time::Delta;
            n = data[frame];
            int32_t d(n["delay"]);
            if (!d) d = 100;
            if (delay >= d) {
                delay -= d;
                if (!(n = data[++frame])) n = data[frame = 0];
            }
            if (n["a0"] || n["a1"]) {
                double a0(n["a0"] ? n["a0"] : 255), a1(n["a1"] ? n["a1"] : 255);
                double dif(double(delay) / d);
                alpha = (a0 * (1 - dif) + a1 * dif) / 255;
            }
        } else n = data;
        if (n.T() != n.bitmap) return;
        Bitmap b(n);
        Node o(n["origin"]);
        uint16_t w(b.Width()), h(b.Height());
        int32_t ox(o.X()), oy(o.Y());
        (flipped ? (x -= w - ox) : (x -= ox)), y -= oy;
        if (view) {
            x += View::Width / 2, y += View::Height / 2;
            x -= View::X, y -= View::Y;
        }
        double ang(0);
        switch (movetype) {
        case 1:
            if (movep) x += movew * sin(Time::TDelta * 2 * M_PI / movep);
            else x += movew * sin(Time::TDelta * 0.001);
            break;
        case 2:
            if (movep) y += moveh * sin(Time::TDelta * 2 * M_PI / movep);
            else y += moveh * sin(Time::TDelta * 0.001);
            break;
        case 3:
            ang = Time::TDelta * 180 / M_PI / mover;
            break;
        }
        glColor4f(1, 1, 1, alpha);
        auto single = [&]() {
            glTranslated(x + ox, y + oy, 0);
            glRotated(ang, 0, 0, 1);
            glTranslated(flipped ? w - ox : -ox, -oy, 0);
            glScaled(flipped ? -w : w, h, 1);
            glDrawArrays(GL_QUADS, 0, 4);
            glLoadIdentity();
        };
        if (tilex) {
            BindTexture(b);
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
                BindTexture(b);
                if (!cy) cy = h;
                int32_t y1 = y % cy - cy;
                int32_t y2 = (y - View::Height) % cy + View::Height + cy;
                for (y = y1; y < y2; y += cy) single();
            } else if (x + w > 0 && x < View::Width && y + h > 0 && y < View::Height) {
                BindTexture(b);
                single();
            }
        }
    }

}