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
    namespace Time {
        uint32_t FPS = 0, TargetFPS = 60;
        double Delta = 1;
        typedef chrono::high_resolution_clock Clock;
        deque<Clock::time_point> LastFrames;
        void Init() {
            LastFrames.push_back(Clock::now());
        }
        void Update() {
            auto last = LastFrames.back();
            auto step = chrono::microseconds(1000000 / TargetFPS);
            this_thread::sleep_until(last + step);
            auto now = max(Clock::now() - step, last + step);
            Delta = chrono::duration_cast<chrono::duration<double>>(now - last).count();
            while (!LastFrames.empty() && now - LastFrames.front() > chrono::seconds(1)) LastFrames.pop_front();
            LastFrames.push_back(now);
            FPS = static_cast<uint32_t>(LastFrames.size());
        }
    }
}
