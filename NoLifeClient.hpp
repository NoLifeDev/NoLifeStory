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
//GLEW
#include <GL/glew.h>
//SFML
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
//Bass
#include <bass.h>
//C Standard Library
#include <cstdint>
//C++ Standard Library
#include <array>
#include <chrono>
#include <deque>

#ifdef __linux__
#include <boost/filesystem.hpp>
using namespace boost::filesystem;
#else
#include <filesystem>
#endif

#include <fstream>
#include <functional>
#include <string>
#include <thread>
using namespace std;

#ifndef __linux__
using namespace std::tr2::sys;
#endif

//NoLifeNx
#include "../NoLifeNx/NX.hpp"
//NoLifeClient
#include "Sound.hpp"
#include "Log.hpp"
#include "Game.hpp"
#include "Map.hpp"
#include "Graphics.hpp"
#include "Time.hpp"
#include "Obj.hpp"
#include "Tile.hpp"
#include "Layer.hpp"
