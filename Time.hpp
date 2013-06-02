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
    namespace Time {
        extern uint32_t FPS, TargetFPS;
        extern double Delta, TDelta;
        extern bool FrameLimit;
        void Init();
        void Update();
    }
    class Timer {
    public:
        Timer() : last(high_resolution_clock::now()) {}
        void DoEvery(high_resolution_clock::duration d, function<void(void)> f) {
            high_resolution_clock::time_point now = high_resolution_clock::now();
            if (now - last > d) {
                f();
                last = now;
            }
        }
    private:
        high_resolution_clock::time_point last;
    };
}