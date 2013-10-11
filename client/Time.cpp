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
#include "NoLifeClient.hpp"
namespace NL {
    namespace Time {
        uint32_t FPS(0);
        double Delta(1), TDelta(0);
        deque<high_resolution_clock::time_point> LastFrames;
        high_resolution_clock::time_point First;
        void Init() {
            First = high_resolution_clock::now();
            LastFrames.push_back(First);
        }
        void Update() {
            high_resolution_clock::time_point last = LastFrames.back();
            if (Config::FrameLimit) sleep_until(last + duration_cast<high_resolution_clock::duration>(seconds(1)) / Config::TargetFPS);
            high_resolution_clock::time_point now = high_resolution_clock::now();
            Delta = duration_cast<duration<double>>(now - last).count();
            if (Delta < 0) Delta = 0;
            if (Delta > 0.05) Delta = 0.05;
            TDelta = duration_cast<duration<double>>(now - First).count();
            while (!LastFrames.empty() && now - LastFrames.front() > seconds(1)) LastFrames.pop_front();
            LastFrames.push_back(now);
            FPS = static_cast<uint32_t>(LastFrames.size());
            Sprite::Unbind();
            glColor4f(1, 0, 0, 1);
            glBegin(GL_LINE_STRIP);
            glVertex2i(0, 2500 / Config::TargetFPS);
            glVertex2i(LastFrames.size() * 2, 2500 / Config::TargetFPS);
            glEnd();
            glColor4f(1, 1, 1, 1);
            glBegin(GL_LINE_STRIP);
            for (size_t i = 1; i < LastFrames.size(); ++i) {
                glVertex2i(i * 2, duration_cast<microseconds>(LastFrames[i] - LastFrames[i - 1]).count() / 400);
            }
            glEnd();
        }
    }
}
