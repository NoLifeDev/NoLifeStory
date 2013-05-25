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
    namespace Player {
        double X = 0, Y = 0;
        void Update() {
            const double mult = 1;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) X -= Time::Delta * mult;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) X += Time::Delta * mult;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) Y -= Time::Delta * mult;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) Y += Time::Delta * mult;
        }
    }
}