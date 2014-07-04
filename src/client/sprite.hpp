//////////////////////////////////////////////////////////////////////////////
// NoLifeClient - Part of the NoLifeStory project                           //
// Copyright © 2014 Peter Atashian                                          //
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

#pragma once
#include <nx/node.hpp>
#include <nx/bitmap.hpp>

namespace nl {
class sprite {
public:
    enum flags : unsigned { none = 0x0, relative = 0x1, flipped = 0x2, tilex = 0x4, tiley = 0x8 };
    sprite() = default;
    sprite(node);
    void draw(int x, int y, flags f, int cx = 0, int cy = 0);
    static void init();
    static void flush();

private:
    void set_frame(int f);
    node data, current;
    bitmap curbit;
    double delay, next_delay;
    double movew, moveh, movep, mover;
    double a0, a1;
    int movetype{0};
    int frame, last_valid{0};
    int originx, originy;
    int width, height;
    bool repeat, animated;
};
sprite::flags &operator|=(sprite::flags &, sprite::flags);
sprite::flags operator|(sprite::flags, sprite::flags);
}
