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
#define _USE_MATH_DEFINES
//GLEW
#include <GL/glew.h>
//SFML
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
//Libmpg123
#include <mpg123.h>
//C Standard Library
#include <cstdint>
//C++ Standard Library
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <deque>
#include <fstream>
#include <functional>
#include <mutex>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
//NoLifeNx (along with platform detection)
#include "../NoLifeNx/NX.hpp"
//Platform Specifics
#ifdef NL_WINDOWS
#  include <filesystem>
#  include <regex>
#  include <Windows.h>
using namespace std::tr2::sys;
#else
#  include <boost/filesystem.hpp>
#  include <boost/regex.hpp>
using namespace boost::filesystem;
using boost::smatch;
using boost::regex_constants;
using boost::regex_match;
using boost::regex;
#endif
#ifdef __X__
#  include <X11/Xlib.h>
#endif
//NoLifeClient
#include "Log.hpp"
#include "Config.hpp"
#include "Sound.hpp"
#include "Sprite.hpp"
#include "View.hpp"
#include "Foothold.hpp"
#include "Game.hpp"
#include "Map.hpp"
#include "Graphics.hpp"
#include "Time.hpp"
#include "Obj.hpp"
#include "Tile.hpp"
#include "Background.hpp"
#include "Layer.hpp"
#include "Physics.hpp"
#include "Player.hpp"
#include "Portal.hpp"
