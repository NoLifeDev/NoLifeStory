//////////////////////////////////////////////////////////////////////////////
// NoLifeClient - Part of the NoLifeStory project                           //
// Copyright (C) 2013 Peter Atashian                             (0)           //
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
    Background::Background(Node n) :
        x(n["x"]), y(n["y"]), z(n["z"]),
        rx(n["rx"]), ry(n["ry"]), cx(n["cx"]), cy(n["cy"]),
        type(n["type"]), flipped(n["f"].GetBool()), 
        spr(NXMap["Back"][n["bS"] + ".img"][n["ani"].GetBool() ? "ani" : "back"][n["no"]]) {
            if (cx < 0) cx = -cx;
            if (cy < 0) cy = -cy;
    }
    void Background::Load() {
        Backgrounds.clear();
        Foregrounds.clear();
        Node b = Map::Current["back"];
        for (size_t i = 0;; ++i) {
            Node n = b[i];
            if (!n) break;
            if (n["front"].GetBool()) Foregrounds.emplace_back(n);
            else Backgrounds.emplace_back(n);
        }
    }
    void Background::Render() {
        int32_t ox = rx * View::X / 100 + View::Width / 2;
        int32_t oy = ry * View::Y / 100 + View::Height / 2;
        switch (type) {
        case 0:
            spr.Draw(x + ox, y + oy, false, flipped, false, false, cx, cy);
            break;
        case 1:
            spr.Draw(x + ox, y + oy, false, flipped, true, false, cx, cy);
            break;
        case 2:
            spr.Draw(x + ox, y + oy, false, flipped, false, true, cx, cy);
            break;
        case 3:
            spr.Draw(x + ox, y + oy, false, flipped, true, true, cx, cy);
            break;
        case 4:
            spr.Draw(x + ox + Time::TDelta * rx * 10, y + oy, false, flipped, true, false, cx, cy);
            break;
        case 5:
            spr.Draw(x + ox, y + oy + Time::TDelta * ry * 10, false, flipped, false, true, cx, cy);
            break;
        case 6:
            spr.Draw(x + ox + Time::TDelta * rx * 10, y + oy, false, flipped, true, true, cx, cy);
            break;
        case 7:
            spr.Draw(x + ox, y + oy + Time::TDelta * ry * 10, false, flipped, true, true, cx, cy);
            break;
        }
    }
    vector<Background> Backgrounds;
    vector<Background> Foregrounds;
}