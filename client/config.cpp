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
#include <GLFW/glfw3.h>
#include <map>
#include <string>
#include <functional>
#include <fstream>
#include <regex>

namespace nl {
    namespace config {
        //Various config variables
        bool rave {false};
        bool fullscreen {false};
        bool vsync {true};
        bool limit_fps {false};
        int32_t target_fps {100};
        int32_t max_textures {4000};
        int32_t window_width {1024}, window_height {768};
        int32_t fullscreen_width {1024}, fullscreen_height {768};
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
        void map_int(std::string const & n, int32_t & v) {
            mappings[n].save = [&v, n] {
                configs[n] = std::to_string(v);
            };
            mappings[n].load = [&v, n] {
                try {
                    v = std::stol(configs[n]);
                } catch (...) {}
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
            for (auto const & m : mappings) m.second.save();
            //Then save the config itself
            std::ofstream file("NoLifeClient.cfg");
            for (auto const & c : configs) {
                file << c.first << " = " << c.second << '\n';
            }
        }
        void load() {
            //First set some runtime defaults
            GLFWvidmode const * mode {glfwGetVideoMode(glfwGetPrimaryMonitor())};
            fullscreen_width = mode->width, fullscreen_height = mode->height;
            target_fps = mode->refreshRate;
            //Then map the configs
            map_bool("rave", rave);
            map_bool("fullscreen", fullscreen);
            map_bool("vsync", vsync);
            map_bool("capfps", limit_fps);
            map_int("fps", target_fps);
            map_int("maxtextures", max_textures);
            map_int("winwidth", window_width);
            map_int("winheight", window_height);
            map_int("fullwidth", fullscreen_width);
            map_int("fullheight", fullscreen_height);
            //First we save the defaults in case the config doesn't have them
            for (auto const & m : mappings) m.second.save();
            //Now we load the config itself
            {
                std::ifstream file {"NoLifeClient.cfg"};
                std::regex reg {"[ \t]*(.*?)[ \t]*=[ \t]*(.*?)[ \t]*", std::regex_constants::optimize};
                std::string line {};
                if (file.is_open()) while (!file.eof()) {
                    getline(file, line);
                    transform(line.cbegin(), line.cend(), line.begin(), [](char const & c) {
                        return std::tolower(c, std::locale::classic());
                    });
                    std::smatch m;
                    if (!std::regex_match(line, m, reg)) continue;
                    configs[m[1]] = m[2];
                }
            }
            for (auto const & m : mappings) m.second.load();
            //And then we save the config back
            save();
        }
    }
}
