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
    namespace Config {
        //Various config variables
        bool Rave = false, Fullscreen = false;
        bool Vsync = true, FrameLimit = false;
        int32_t TargetFPS = 100;
        int32_t WindowWidth = 1024, WindowHeight = 768;
        int32_t FullscreenWidth = 1024, FullscreenHeight = 768;
        //Stuff to hold configs and their mappings
        map<string, string> Configs;
        map<string, pair<function<void(void)>, function<void(void)>>> Mappings;
        //These mappings provide an easy way to map a variable to a config
        void StringMapping(string n, string & v) {
            Mappings[n].first = [&v, n]{Configs[n] = v;};
            Mappings[n].second = [&v, n]{v = Configs[n];};
        }
        void IntMapping(string n, int32_t & v) {
            Mappings[n].first = [&v, n]{Configs[n] = to_string(v);};
            Mappings[n].second = [&v, n]{try {v = stol(Configs[n]);} catch (...){}};
        }
        void BoolMapping(string n, bool & v) {
            Mappings[n].first = [&v, n]{Configs[n] = (v ? "true" : "false");};
            Mappings[n].second = [&v, n]{v = (Configs[n] == "true" ? true : false);};
        }
        void Save() {
            //Write the variables to their configs
            for (auto & m : Mappings) m.second.first();
            //Then save the config itself
            ofstream file("NoLifeClient.cfg");
            for (auto && c : Configs) {
                file << c.first << " = " << c.second << endl;
            }
        }
        void Load() {
            //First set some runtime defaults
            auto v = sf::VideoMode::getFullscreenModes()[0];
            FullscreenWidth = v.width, FullscreenHeight = v.height;
#ifdef NL_WINDOWS
            DEVMODEA dev = {};
            dev.dmSize = sizeof(DEVMODEA);
            dev.dmDriverExtra = 0;
            EnumDisplaySettingsA(nullptr, ENUM_CURRENT_SETTINGS, &dev);
            TargetFPS = dev.dmDisplayFrequency;
#endif
            //Then map the configs
            IntMapping("winwidth", WindowWidth);
            IntMapping("winheight", WindowHeight);
            IntMapping("fullwidth", FullscreenWidth);
            IntMapping("fullheight", FullscreenHeight);
            IntMapping("fps", TargetFPS);
            BoolMapping("fullscreen", Fullscreen);
            BoolMapping("rave", Rave);
            BoolMapping("vsync", Vsync);
            BoolMapping("capfps", FrameLimit);
            //First we save the defaults in case the config doesn't have them
            for (auto & m : Mappings) m.second.first();
            //Now we load the config itself
            {
                ifstream file("NoLifeClient.cfg");
                regex reg("[ \t]*(.*?)[ \t]*=[ \t]*(.*?)[ \t]*");
                if (file.is_open()) while (!file.eof()) {
                    string s;
                    getline(file, s);
                    transform(s.begin(), s.end(), s.begin(), [](char c){return tolower(c);});
                    smatch m;
                    if (!regex_match(s, m, reg)) continue;
                    Configs[m[1]] = m[2];
                }
            }
            for (auto & m : Mappings) m.second.second();
            //And then we save the config
            Save();
        }
    }
}