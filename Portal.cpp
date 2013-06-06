//////////////////////////////////////////////////////////////////////////////
// NoLifeClient - Part of the NoLifeStory project                           //
// Copyright (C) 2013 Peter Atashian                                        //
// Additional Authors                                                       //
// 2013 Cedric Van Goethem                                                  //
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
    vector<Portal> Portals;
    Node PortalSprites;
    void Portal::Load() {
        PortalSprites = NXMap["MapHelper.img"]["portal"]["game"];
        Portals.clear();
        for (Node n : Map::Current["portal"]) Portals.emplace_back(n);
    }
    Portal::Portal(Node n) :
        x(n["x"]), y(n["y"]), tm(n["tm"]), tn(n["tn"]),
        pt(n["pt"]), pn(n["pn"]) {
        switch (pt) {
        case 2:
            spr = PortalSprites["pv"];
            break;
        case 10:
            spr = PortalSprites["ph"]["default"];
            break;
        case 11:
            spr = PortalSprites["psh"]["default"];
            break;
        }
    }

    bool Portal::IsInRange() {
        return abs(NL::Player::Pos.x - x) < 60 && abs(NL::Player::Pos.y - y) < 60;
    }

    bool Portal::Check() {
        bool use = false;

        switch (pt) {
        case 1:
        case 2:
        case 10:
            use = IsInRange() && sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
            break;
        case 3:
            use = IsInRange();
            break;
        }

        if(use)
            Use();

        return use;
    }

    void Portal::Use() {
        if(tm != 999999999) {
            NL::Map::Load(to_string(tm), &string(tn)); // To other map
        } else {
            NL::Player::Respawn(&string(tn));
        }
    }

    void Portal::Render() {
        switch (pt) {
        case 2:
        case 10:
        case 11:
            spr.Draw(x, y, true, false);
            break;
        }
    }
}