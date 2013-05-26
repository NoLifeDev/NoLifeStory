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
        int32_t FPS(0), TargetFPS(65), Delta(1);
        typedef high_resolution_clock Clock;
        deque<Clock::time_point> LastFrames;
        void Init() {
            LastFrames.push_back(Clock::now());
        }
        void Update() {
            Clock::time_point last = LastFrames.back();
            milliseconds step = milliseconds(1000) / TargetFPS;
            Delta = step.count();
            sleep_until(last + step);
            Clock::time_point now = max(Clock::now() - step, last + step);
            while (!LastFrames.empty() && now - LastFrames.front() > seconds(1)) LastFrames.pop_front();
            LastFrames.push_back(now);
            FPS = static_cast<uint32_t>(LastFrames.size());
        }
    }
}
