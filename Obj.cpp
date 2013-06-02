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
    Obj::Obj(Node n) :
        x(n["x"]), y(n["y"]), z(n["z"]),
        rx(n["rx"]), ry(n["ry"]), flip(n["f"].GetBool()), 
        cx(n["cx"]), cy(n["cy"]), flow(n["flow"]),
        data(NXMap["Obj"][n["oS"] + ".img"][n["l0"]][n["l1"]][n["l2"]]) {
            if (!cx) cx = View::Right - View::Left;
            if (!cy) cy = View::Bottom - View::Top;
            if (cx < 0) cx = -cx;
            if (cy < 0) cy = -cy;
    }
    bool Obj::operator<(Obj const & o) const {
        return z < o.z;
    }
    void Obj::Render() {
        switch (flow) {
        case 0:
            data.Draw(x, y, true, flip, false, false, 0, 0);
            break;
        case 1:
            data.Draw(x + Time::TDelta * rx * 10, y, true, flip, true, false, cx, 0);
            break;
        case 2:
            data.Draw(x, y + Time::TDelta * ry * 10, true, flip, false, false, 0, cy);
            break;
        case 3:
            data.Draw(x + Time::TDelta * rx * 10, y + Time::TDelta * ry * 10, true, flip, true, false, cx, cy);
            break;
        }
    }
}