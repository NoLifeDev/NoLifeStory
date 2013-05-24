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
    array<Layer, 8> Layers;
    void Layer::RenderAll() {
        for (Layer & l : Layers) {
            for (Obj & o : l.Objs) o.Render();
        }
    }
    void Layer::LoadAll() {
        for (int i = 0; i < 8; ++i) {
            Layer & l = Layers[i];
            Node n = Map::Current[i];
            l.Objs.clear();
            for (Node nn : n["obj"]) l.Objs.emplace_back(nn);
            sort(l.Objs.begin(), l.Objs.end());
        }
    }
}