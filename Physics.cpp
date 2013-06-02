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
    Physics::Physics() : x(0), y(0), layer(7) {}
    void Physics::Update() {
        const double mult(1000);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) x -= Time::Delta * mult;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) x += Time::Delta * mult;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) y -= Time::Delta * mult;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) y += Time::Delta * mult;
        if (x < View::Left) x = View::Left;
        if (x > View::Right) x = View::Right;
        if (y < View::Top) y = View::Top;
        if (y > View::Bottom) y = View::Bottom;
    }
}