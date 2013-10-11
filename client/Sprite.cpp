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
    size_t LastBound(0);
    mutex LoadMutex;;
    mutex SpriteMutex;
    set<Bitmap> SpritesToLoad;
    atomic<bool> ThreadContextMade(false);
    void SpriteThread() {
        {
            sf::Context context;
            ThreadContextMade = true;
            while (!Game::Over) {
                for (;;) {
                    Bitmap b;
                    {
                        lock_guard<mutex> lock(LoadMutex);
                        if (SpritesToLoad.empty()) break;
                        b = *SpritesToLoad.begin();
                    }
                    GLuint t;
                    glGenTextures(1, &t);
                    glBindTexture(GL_TEXTURE_2D, t);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, b.Width(), b.Height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, b.Data());
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    {
                        lock_guard<mutex> lock(LoadMutex);
                        SpritesToLoad.erase(b);
                        Sprites[b.ID()] = t;
                        LoadedSprites.push_back(b.ID());
                    }
                }
                glFlush();
                sleep_for(milliseconds(10));
            }
            SpriteMutex.lock();
        }
        SpriteMutex.unlock();
    }
    void Sprite::Init() {
        if (!Config::Threaded) return;
        thread([] {SpriteThread(); }).detach();
        while (!ThreadContextMade) sleep_for(milliseconds(1));
        SpriteMutex.lock();
    }
    void Sprite::Unbind() {
        LastBound = 0;
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    void Sprite::Cleanup() {
        lock_guard<mutex> lock(LoadMutex);
        while (LoadedSprites.size() > Config::MaxTextures) {
            size_t s = LoadedSprites.front();
            LoadedSprites.pop_front();
            glDeleteTextures(1, &Sprites[s]);
            Sprites.erase(s);
        }
    }
    bool BindTexture(Bitmap b) {
        if (b.ID() == LastBound) return true;
        GLuint t;
        {
            lock_guard<mutex> lock(LoadMutex);
            t = Sprites[b.ID()];
        }
        if (t) {
            LastBound = b.ID();
            glBindTexture(GL_TEXTURE_2D, t);
            if (!glIsTexture(t)) throw "Invalid texture detected!";
            return true;
        }
        if (!Config::Threaded) {
            GLuint t;
            glGenTextures(1, &t);
            glBindTexture(GL_TEXTURE_2D, t);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, b.Width(), b.Height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, b.Data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            LastBound = b.ID();
            Sprites[b.ID()] = t;
            LoadedSprites.push_back(b.ID());
            return true;
        } else {
            lock_guard<mutex> lock(LoadMutex);
            SpritesToLoad.insert(b);
        }
        return false;
    }
    Sprite::Sprite() : frame(0), delay(0), data(), last(), next() {}
    Sprite::Sprite(Node const &o) : frame(0), delay(0), data(o), last(), next() {
        last = next = data.T() == Node::Type::Bitmap ? data : data["0"];
        movetype = next["moveType"];
        movew = next["moveW"];
        moveh = next["moveH"];
        movep = next["moveP"];
        mover = next["moveR"];
        repeat = next["repeat"].GetBool();
        
        Bitmap b = last;
        Width = b.Width();
        Height = b.Height();
    }
    void Sprite::Draw(int32_t x, int32_t y, bool view, bool flipped, bool tilex, bool tiley, int32_t cx, int32_t cy) {
        if (!data) return;
        float alpha(1);
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
        }
    }

}