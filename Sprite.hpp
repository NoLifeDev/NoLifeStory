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
#pragma once
namespace NL {
    extern mutex SpriteMutex;
    class Sprite {
    public:
        Sprite();
        Sprite(Node const &);
        void Draw(int32_t x, int32_t y, bool view, bool flipped, bool tilex = false, bool tiley = false, int32_t cx = 0, int32_t cy = 0);
        static void Init();
        static void Cleanup();
        static void Unbind();
        uint32_t Width, Height;
    private:
        int32_t frame;
        double delay;
        Node data, last, next;
        int32_t movetype;
        double movew, moveh, movep, mover;
        bool repeat;
    };
}