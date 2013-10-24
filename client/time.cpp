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

#include "time.hpp"
#include "config.hpp"
#include <GL/glew.h>
#include <deque>
#include <chrono>
#include <thread>

namespace nl {
    namespace time {
        uint32_t fps {0};
        double delta {1}, delta_total {0};
        std::deque<std::chrono::high_resolution_clock::time_point> frames {};
        std::chrono::high_resolution_clock::time_point first {};
        void init() {
            first = std::chrono::high_resolution_clock::now();
            frames.push_back(first);
        }
        void draw() {
            //Sprite::Unbind();
            glColor4f(1, 0, 0, 1);
            glBegin(GL_LINE_STRIP);
            glVertex2i(0, 2500 / config::target_fps);
            glVertex2i(static_cast<GLint>(frames.size() * 2), static_cast<GLint>(2500 / config::target_fps));
            glEnd();
            glColor4f(1, 1, 1, 1);
            glBegin(GL_LINE_STRIP);
            for (size_t i {1}; i < frames.size(); ++i) {
                glVertex2i(static_cast<GLint>(i * 2), static_cast<GLint>(std::chrono::duration_cast<std::chrono::microseconds>(frames[i] - frames[i - 1]).count() / 400));
            }
            glEnd();
        }
        void update() {
            std::chrono::high_resolution_clock::time_point last {frames.back()};
            if (config::target_fps <= 0) config::target_fps = 1;//Because some people are idiots
            std::chrono::high_resolution_clock::duration step_size {std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(std::chrono::seconds {1}) / config::target_fps};
            if (config::limit_fps) std::this_thread::sleep_until(last + step_size - std::chrono::milliseconds {10});
            std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
            if (config::limit_fps && now - last < step_size) now = last + step_size;
            delta = std::chrono::duration_cast<std::chrono::duration<double>>(now - last).count();
            if (delta < 0) delta = 0;
            if (delta > 0.05) delta = 0.05;
            delta_total = std::chrono::duration_cast<std::chrono::duration<double>>(now - first).count();
            while (!frames.empty() && now - frames.front() > std::chrono::seconds {1}) frames.pop_front();
            frames.push_back(now);
            fps = static_cast<uint32_t>(frames.size());
            draw();
        }
    }
}
