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
#include "log.hpp"
#include "window.hpp"
#include <nx/bitmap.hpp>
#include <GL/glew.h>
#include <array>
#include <chrono>
#include <cmath>
#include <complex>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace nl {
    namespace {
        struct texture {
            GLfloat top, left, bottom, right, layer;
            std::chrono::steady_clock::time_point last_use;
        };
        struct block {
            GLint x, y, z;
            GLint size;
        };
        struct vertex {
            GLfloat r, g, b, a;
            GLfloat x, y;
            GLfloat s, t, p;
        };
        double const tau{6.28318530717958647692528676655900576839433879875021};
        std::unordered_map<size_t, texture> textures{};
        std::array<std::vector<block>, 32> blocks{};
        bool bound{false};
        GLuint vbo = 0;
        GLuint atlas{};
        GLint atlas_size{};
        GLint layers{1};
        std::vector<vertex> vertices{};
        GLint npot(GLint n) {
            --n;
            n |= n >> 1;
            n |= n >> 2;
            n |= n >> 4;
            n |= n >> 8;
            n |= n >> 16;
            ++n;
            return n;
        }
        size_t lzcnt(GLint n) {
#ifndef _MSC_VER
            size_t i{32};
            while (n) {
                n >>= 1;
                --i;
            }
            return i;
#else
            return __lzcnt(static_cast<unsigned>(n));
#endif
        }
        void reinit() {
            glEnable(GL_TEXTURE_3D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindTexture(GL_TEXTURE_3D, atlas);
            glLoadIdentity();
            vertices.clear();
            bound = true;
        }
        void reset_blocks() {
            for (auto & b : blocks) {
                b.clear();
            }
            auto maxblock = 31 - lzcnt(atlas_size);
            for (auto i = 0; i < layers; ++i) {
                blocks[maxblock].push_back({0, 0, i, atlas_size});
            }
        }
        void carve_block(block const & p_block, GLint p_width, GLint p_height) {
            if (p_block.size <= 8) {
                return;
            }
            if (p_width > p_block.size && p_height > p_block.size) {
                return;
            }
            if (p_width <= 0 || p_height <= 0) {
                blocks[31 - lzcnt(p_block.size)].push_back(p_block);
                return;
            }
            auto s = p_block.size / 2;
            carve_block({p_block.x, p_block.y, p_block.z, s},
                        p_width, p_height);
            carve_block({p_block.x + s, p_block.y, p_block.z, s},
                        p_width - s, p_height);
            carve_block({p_block.x + s, p_block.y + s, p_block.z, s},
                        p_width - s, p_height - s);
            carve_block({p_block.x, p_block.y + s, p_block.z, s},
                        p_width, p_height - s);
        }
        block get_block(GLint p_width, GLint p_height) {
            auto size = npot(std::max(p_width, p_height));
            if (size > atlas_size) {
                throw std::runtime_error{"Texture is too big"};
            }
            if (!size) {
                throw std::runtime_error{"Invalid texture size"};
            }
            auto index = 31 - lzcnt(size);
            for (auto i = index; i < blocks.size(); ++i) {
                if (!blocks[i].empty()) {
                    auto b = blocks[i].back();
                    blocks[i].pop_back();
                    carve_block(b, p_width, p_height);
                    return b;
                }
            }
            sprite::flush();
            log << "Wiping texture atlas" << std::endl;
            reset_blocks();
            textures.clear();
            return get_block(p_width, p_height);
        }
        texture & get_texture(bitmap const & p_bitmap) {
            auto it = textures.find(p_bitmap.id());
            if (it != textures.end()) {
                if (!bound) {
                    reinit();
                }
                it->second.last_use = std::chrono::steady_clock::now();
                return it->second;
            }
            auto bl = get_block(p_bitmap.width(), p_bitmap.height());
            if (!bound) {
                reinit();
            }
            glTexSubImage3D(GL_TEXTURE_3D, 0, bl.x, bl.y, bl.z, p_bitmap.width(), p_bitmap.height(), 1, GL_BGRA, GL_UNSIGNED_BYTE, p_bitmap.data());
            auto & tex = textures[p_bitmap.id()];
            auto sf = static_cast<GLfloat>(atlas_size);
            tex.left = bl.x / sf;
            tex.right = (bl.x + p_bitmap.width()) / sf;
            tex.top = bl.y / sf;
            tex.bottom = (bl.y + p_bitmap.height()) / sf;
            tex.layer = bl.z / static_cast<GLfloat>(layers);
            tex.last_use = std::chrono::steady_clock::now();
            return tex;
        }
    }
    void sprite::init() {
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &atlas_size);
        glGenTextures(1, &atlas);
        glBindTexture(GL_TEXTURE_3D, atlas);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, atlas_size, atlas_size, layers, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        reset_blocks();
        glGenBuffers(1, &vbo);
    }
    void sprite::flush() {
        if (bound) {
            bound = false;
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(vertex)), vertices.data(), GL_STREAM_DRAW);
            glEnableClientState(GL_COLOR_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glColorPointer(4, GL_FLOAT, sizeof(vertex), reinterpret_cast<GLvoid const *>(0 * sizeof(GLfloat)));
            glVertexPointer(2, GL_FLOAT, sizeof(vertex), reinterpret_cast<GLvoid const *>(4 * sizeof(GLfloat)));
            glTexCoordPointer(3, GL_FLOAT, sizeof(vertex), reinterpret_cast<GLvoid const *>(6 * sizeof(GLfloat)));
            glDrawArrays(GL_QUADS, 0, static_cast<GLsizei>(vertices.size()));
        }
    }
    sprite::sprite(node o) : data(o) {
        if (data.data_type() == node::type::bitmap)
            animated = false;
        else if (data["0"].data_type() == node::type::bitmap)
            animated = true;
        else
            data = {};
        set_frame(0);
    }
    void sprite::set_frame(int f) {
        if (!data)
            return;
        frame = f;
        if (!animated) {
            current = data;
        } else {
            current = data[frame];
            if (!current)
                current = data[frame = 0];
            delay = 0;
            next_delay = current["delay"].get_real(100);
        }
        curbit = current;
        if (current["source"]) {
            std::string str = current["source"];
            auto n = current.root().resolve(str.substr(str.find_first_of('/') + 1));
            if (n.data_type() == node::type::bitmap)
                curbit = n;
        }
        if (!curbit)
            return;
        width = curbit.width();
        height = curbit.height();
        auto o = current["origin"];
        originx = o.x();
        originy = o.y();
        if (current["moveType"]) {
            movetype = current["moveType"];
            movew = current["moveW"];
            moveh = current["moveH"];
            movep = current["moveP"].get_real(1000 * tau);
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
        if (!data)
            return;
        if (animated) {
            delay += time::delta * 1000;
            if (delay > next_delay)
                set_frame(frame + 1);
        }
        if (!curbit)
            return;
        //cx and cy represent tiling distance
        if (!cx)
            cx = width;
        else if (cx < 0)
            cx = -cx;
        if (!cy)
            cy = height;
        else if (cy < 0)
            cy = -cy;
        //A bit of origin and view shifting
        if (f & flipped)
            x -= width - originx;
        else
            x -= originx;
        y -= originy;
        if (f & relative) {
            x -= view::xmin;
            y -= view::ymin;
        }
        //Handling movetypes
        auto angle = 0.;
        switch (movetype) {
        case 0:
            break;
        case 1:
            x += static_cast<int>(movew * sin(tau * 1000 * time::delta_total / movep));
            break;
        case 2:
            y += static_cast<int>(moveh * sin(tau * 1000 * time::delta_total / movep));
            break;
        case 3:
            angle = tau * 1000 * time::delta_total / mover;
            break;
        default:
            log << "Unknown move type: " << movetype << std::endl;
        }
        auto xbegin = x;
        auto xend = x;
        auto ybegin = y;
        auto yend = y;
        if (f & tilex) {
            xbegin += width;
            xbegin %= cx;
            if (xbegin <= 0)
                xbegin += cx;
            xbegin -= width;
            xend -= view::width;
            xend %= cx;
            if (xend >= 0)
                xend -= cx;
            xend += view::width;
            if (xend < xbegin)
                return;
        }
        if (f & tiley) {
            ybegin += height;
            ybegin %= cy;
            if (ybegin <= 0)
                ybegin += cy;
            ybegin -= height;
            yend -= view::height;
            yend %= cy;
            if (yend >= 0)
                yend -= cy;
            yend += view::height;
            if (yend < ybegin)
                return;
        }
        if (xend + width < 0)
            return;
        if (xbegin > view::width)
            return;
        if (yend + height < 0)
            return;
        if (ybegin > view::height)
            return;
        GLfloat alpha{1};
        if (animated) {
            auto dif = delay / next_delay;
            alpha = static_cast<GLfloat>(dif * a1 + (1 - dif) * a0);
        }
        auto & tex = get_texture(curbit);
        for (x = xbegin; x <= xend; x += cx) {
            for (y = ybegin; y <= yend; y += cy) {
                std::complex<double> vex[4] = {
                    {0, 0},
                    {static_cast<double>(width), 0},
                    {static_cast<double>(width), static_cast<double>(height)},
                    {0, static_cast<double>(height)}};
                if (angle != 0) {
                    std::complex<double> rot{std::cos(angle), std::sin(angle)};
                    for (auto & v : vex) {
                        v *= rot;
                    }
                }
                std::complex<double> pos{static_cast<double>(x), static_cast<double>(y)};
                for (auto & v : vex) {
                    v += pos;
                }
                vertices.push_back({1, 1, 1, alpha,
                                   static_cast<GLfloat>(vex[0].real()), static_cast<GLfloat>(vex[0].imag()),
                                   f & flipped ? tex.right : tex.left, tex.top, tex.layer});
                vertices.push_back({1, 1, 1, alpha,
                                   static_cast<GLfloat>(vex[1].real()), static_cast<GLfloat>(vex[1].imag()),
                                   f & flipped ? tex.left : tex.right, tex.top, tex.layer});
                vertices.push_back({1, 1, 1, alpha,
                                   static_cast<GLfloat>(vex[2].real()), static_cast<GLfloat>(vex[2].imag()),
                                   f & flipped ? tex.left : tex.right, tex.bottom, tex.layer});
                vertices.push_back({1, 1, 1, alpha,
                                   static_cast<GLfloat>(vex[3].real()), static_cast<GLfloat>(vex[3].imag()),
                                   f & flipped ? tex.right : tex.left, tex.bottom, tex.layer});
            }
        }
    }
    sprite::flags & operator|=(sprite::flags & a, sprite::flags b) {
        return a = static_cast<sprite::flags>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
    }
    sprite::flags operator|(sprite::flags a, sprite::flags b) {
        return static_cast<sprite::flags>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
    }
}
