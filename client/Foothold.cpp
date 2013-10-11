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
#include "NoLifeClient.hpp"
namespace NL {
    vector<Foothold> Footholds;
    void Foothold::Construct(Node n, int32_t id, int32_t group, int32_t layer) {
        x1 = n["x1"];
        x2 = n["x2"];
        y1 = n["y1"];
        y2 = n["y2"],
        force = n["force"];
        piece = n["piece"];
        nextid = n["next"];
        previd = n["prev"];
        cantThrough = n["cantThrough"].GetBool();
        forbidFallDown = n["forbidFallDown"].GetBool();
        this->id = id;
        this->group = group;
        this->layer = layer;
        next = nullptr;
        prev = nullptr;
    }
    void Foothold::Load() {
        Footholds.clear();
        size_t s = 0;
        for (Node layer : Map::Current["foothold"]) for (Node group : layer) for (Node id : group) s = max<size_t>(stol(id.Name()), s);
        Footholds.resize(s + 1);
        for (Node layer : Map::Current["foothold"]) for (Node group : layer) for (Node id : group) {
            Footholds[stol(id.Name())].Construct(id, stol(id.Name()), stol(group.Name()), stol(layer.Name()));
        }
        for (Foothold & f : Footholds) {
            if (f.nextid) f.next = &Footholds[f.nextid];
            if (f.previd) f.prev = &Footholds[f.previd];
        }
    }
}