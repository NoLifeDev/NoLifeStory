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

#include "game.hpp"

namespace nl {
    namespace game {
        bool over {false};
        void play() {
            //Config::Load();
            //LoadAllNX();
            //Time::Init();
            //Graphics::Init();
            //Sprite::Init();
            //ClassicUI::Init();
            //Map::Init();
            while (!over) {
                //Player::Update();
                //View::Update();
                //Map::Render();
                //ClassicUI::Render();
                //Time::Update();
                //Graphics::Update();
            }
            //BGM.stop();
            //Graphics::Unload();
            //Config::Save();
            //if (Config::Threaded) SpriteMutex.unlock();
            //sleep_for(seconds(1));//To let threads finish safely
        }
    }
}
