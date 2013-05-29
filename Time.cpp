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
        bool FrameLimit(true);
        typedef high_resolution_clock Clock;
        typedef time_point<high_resolution_clock, milliseconds> Time_Point;
        deque<Time_Point> LastFrames;
        void Init() {
            LastFrames.push_back(time_point_cast<milliseconds>(Clock::now()));
        }
        void Update() {
            Time_Point last = LastFrames.back();
            milliseconds step = milliseconds(1000) / TargetFPS;
            if (FrameLimit) sleep_until(last + step);
            Time_Point now = time_point_cast<milliseconds>(Clock::now());
            now = min(now, max(last + step, now - step));
            Delta = duration_cast<milliseconds>(now - last).count();
            while (!LastFrames.empty() && now - LastFrames.front() > seconds(1)) LastFrames.pop_front();
            LastFrames.push_back(now);
            FPS = static_cast<uint32_t>(LastFrames.size());
        }
    }
}
