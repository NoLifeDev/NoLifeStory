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

#include "config.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>
#include <string>
#include <functional>
#include <fstream>
#include <regex>

namespace nl {
    namespace config {
        //Various config variables
        bool stretch = false;
        bool rave = false;
        bool fullscreen = false;
        bool vsync = true;
        bool limit_fps = false;
        int target_fps = 100;
        int window_width = 1024, window_height = 768;
        int fullscreen_width = 1024, fullscreen_height = 768;
        int atlas_size = 0;
        std::string map{"100000000"};
        //Stuff to hold configs and their mappings
        struct mapping {
            std::function<void()> save;
            std::function<void()> load;
        };
        std::map<std::string, std::string> configs;
        std::map<std::string, mapping> mappings;
        //These mappings provide an easy way to map a variable to a config
        void map_string(std::string const & n, std::string & v) {
            mappings[n].save = [&v, n] {
                configs[n] = v;
            };
            mappings[n].load = [&v, n] {
                v = configs[n];
            };
        }
        void map_int(std::string const & n, int & v) {
            mappings[n].save = [&v, n] {
                configs[n] = std::to_string(v);
            };
            mappings[n].load = [&v, n] {
                //Try catch this to prevent crashes from user stupidity
                try {
                    v = std::stoi(configs[n]);
                } catch (std::exception const &) {}
            };
        }
        void map_bool(std::string const & n, bool & v) {
            mappings[n].save = [&v, n] {
                configs[n] = v ? "true" : "false";
            };
            mappings[n].load = [&v, n] {
                v = configs[n] == "true" ? true : false;
            };
        }
        void save() {
            //Write the variables to their configs
            for (auto const & m : mappings)
                m.second.save();
            //Then save the config itself
            std::ofstream file("NoLifeClient.cfg");
            for (auto const & c : configs)
                file << c.first << " = " << c.second << '\n';
        }
        void load() {
            //First set some runtime defaults
            auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            fullscreen_width = mode->width;
            fullscreen_height = mode->height;
            target_fps = mode->refreshRate;
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &atlas_size);
            //Then map the configs
            map_bool("rave", rave);
            map_bool("fullscreen", fullscreen);
            map_bool("vsync", vsync);
            map_bool("capfps", limit_fps);
            map_bool("stretch", stretch);
            map_int("fps", target_fps);
            map_int("winwidth", window_width);
            map_int("winheight", window_height);
            map_int("fullwidth", fullscreen_width);
            map_int("fullheight", fullscreen_height);
            map_int("atlassize", atlas_size);
            map_string("map", map);
            //First we save the defaults in case the config file doesn't have them
            for (auto const & m : mappings)
                m.second.save();
            //Now we load the config itself
            std::ifstream file("NoLifeClient.cfg");
            std::regex reg("[ \t]*([a-z0-9.-]+)[ \t]*=[ \t]*([a-z0-9.-]+)[ \t]*", std::regex_constants::extended | std::regex_constants::optimize);
            std::string line;
            if (file.is_open())
            while (!file.eof()) {
                getline(file, line);
                transform(line.cbegin(), line.cend(), line.begin(), [](char const & c) {
                    return std::tolower(c, std::locale::classic());
                });
                std::smatch m;
                if (!std::regex_match(line, m, reg))
                    continue;
                configs[m[1]] = m[2];
            }
            for (auto const & m : mappings)
                m.second.load();
            //And then we save the config back
            save();
        }
    }
}
