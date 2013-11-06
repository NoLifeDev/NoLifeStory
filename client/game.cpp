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

#include "game.hpp"
#include "window.hpp"
#include "config.hpp"
#include "time.hpp"
#include "view.hpp"
#include "map.hpp"
#include <nx/nx.hpp>
#include <iostream>

namespace nl {
    namespace game {
        bool over = false;
        void init() {
            window::init();
            config::load();
            nx::load_all();
            time::init();
            window::recreate(config::fullscreen);
            map::init();
        }
        void loop() {
            time::update();
            view::update();
            map::update();
            map::render();
            window::update();
        }
        void unload() {
            config::save();
            window::unload();
        }
        void play() {
            init();
            while (!over) loop();
            unload();
        }
        void shut_down() {
            over = true;
        }
        bool is_over() {
            return over;
        }
    }
}
