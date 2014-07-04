//////////////////////////////////////////////////////////////////////////////
// NoLifeClient - Part of the NoLifeStory project                           //
// Copyright © 2014 Peter Atashian                                          //
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

#include "game.hpp"
#include "window.hpp"
#include "config.hpp"
#include "time.hpp"
#include "view.hpp"
#include "map.hpp"
#include "sound.hpp"
#include "player.hpp"
#include "sprite.hpp"
#include <nx/nx.hpp>

namespace nl {
namespace game {
bool over = false;
void init() {
    window::init();
    config::load();
    sprite::init();
    time::reset();
    nx::load_all();
    music::init();
    window::recreate(config::fullscreen);
    map::init();
    player::init();
}
void loop() {
    view::update();
    map::update();
    map::render();
    time::update();
    window::update();
}
void unload() {
    music::unload();
    config::save();
    window::unload();
}
void play() {
    init();
    while (!over) loop();
    unload();
}
void shut_down() { over = true; }
bool is_over() { return over; }
}
}
