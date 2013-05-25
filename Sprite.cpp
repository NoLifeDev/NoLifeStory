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
    map<size_t, GLuint> Sprites;
    void BindTexture(Bitmap b) {
        GLuint t = Sprites[b.ID()];
        if (t) {
            glBindTexture(GL_TEXTURE_2D, t);
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
    Sprite::Sprite(Sprite const & o) : frame(o.frame), delay(o.delay), data(o.data) {}
    Sprite::Sprite(Node const &o) : frame(0), delay(0), data(o) {}
    Sprite & Sprite::operator=(Sprite const & o) {
        frame = o.frame;
        delay = o.delay;
        data = o.data;
    }
    void Sprite::Draw(int32_t x, int32_t y) {
        if (data.T() == data.bitmap) {
            Bitmap b = data;
            BindTexture(b);
            glBegin(GL_QUADS);
            glTexCoord2i(0, 0);
            glVertex2i(x - data["origin"].X(),             y - data["origin"].Y());
            glTexCoord2i(1, 0);
            glVertex2i(x - data["origin"].X() + b.Width(), y - data["origin"].Y());
            glTexCoord2i(1, 1);
            glVertex2i(x - data["origin"].X() + b.Width(), y - data["origin"].Y() + b.Height());
            glTexCoord2i(0, 1);
            glVertex2i(x - data["origin"].X(),             y - data["origin"].Y() + b.Height());
            glEnd();
        } else {
            double movew, moveh, movep, mover;
            int32_t movetype;
            bool repeat;
            //movetype = data["moveType"];
            //movew = data["moveW"];
            //moveh = data["moveH"];
            //movep = data["moveP"];
            //mover = data["moveR"];
            //repeat = data["repeat"];
        }
    }
}