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

#pragma once
#include <nx/node.hpp>
#include <cstdint>

namespace nl {
    class sprite {
    public:
        enum flags {
            none = 0x0,
            relative = 0x1,
            flipped = 0x2,
            tilex = 0x4,
            tiley = 0x8
        };
        sprite();
        sprite(node);
        void draw(int32_t x, int32_t y, flags f, int32_t cx = 0, int32_t cy = 0);
        static void init();
        static void cleanup();
        static void unbind();
        static void reinit();
    private:
        void set_frame(int32_t f);
        void bind();
        node data, current;
        double delay, next_delay;
        double movew, moveh, movep, mover;
        int32_t movetype;
        int32_t frame;
        int32_t originx, originy;
        uint16_t width, height;
        bool repeat, animated;
    };
}
