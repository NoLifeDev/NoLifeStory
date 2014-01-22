//////////////////////////////////////////////////////////////////////////////
// NoLifeNxBench - Part of the NoLifeStory project                          //
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

#include <nx/nx.hpp>
#include <nx/node.hpp>
#include <vector>

using namespace nl;
void fraysa() {
    using byte = unsigned char;
    nx::load_all();
    struct item {
        int mapleid;
        int saleprice;
        int cbufftime;
        short maxperstack;
        short requiredlevel;
        short chp;
        short cmp;
        short chppercentage;
        short cmppercentage;
        short cweaponattack;
        short cweapondefense;
        short cmagicattack;
        short cmagicdefense;
        short caccuracy;
        short cavoid;
        bool istradeblocked;
    };
    auto shops = std::vector<int>();
    auto reactor_states = std::vector<std::vector<byte>>();
    auto items = std::vector<item>();
    for (auto n1 : nx::npc) 
        if (n1["info"]["shop"].get_integer() > 0)
            shops.push_back(std::stoi(n1.name()));
    for (auto n1 : nx::reactor) {
        auto states = std::vector<byte>();
        for (auto n2 : n1) {
            auto s = n2.name();
            if (s != "action" && s != "info" && s != "quest" && s != "origin" && s != "z")
                states.push_back(std::stoi(s));
        }
        states.push_back(std::numeric_limits<byte>::max());
        reactor_states.push_back(std::move(states));
    }
    for (auto n1 : nx::item) {
        auto s = n1.name();
        if (s != "ItemOption.img" && s != "Pet" && s != "ThothSearchOption.img")
            for (auto n2 : n1)
                for (auto n3 : n2) {
                    items.emplace_back();
                    auto & i = items.back();
                    i.mapleid = std::stoi(n3.name());
                    auto n4 = n3["info"];
                    i.maxperstack = n4["slotMax"].get_integer(1);
                    i.saleprice = n4["price"].get_integer(1);
                    i.cweaponattack = n4["incPAD"];
                    i.requiredlevel = n4["reqLevel"];
                    i.istradeblocked = n4["tradeBlock"].get_integer();
                    auto n5 = n3["spec"];
                    i.chp = n5["hp"];
                    i.cmp = n5["mp"];
                    i.chppercentage = n5["hpR"];
                    i.cmppercentage = n5["mpR"];
                    i.cweaponattack = n5["pad"];
                    i.cweapondefense = n5["pdd"];
                    i.cmagicattack = n5["mad"];
                    i.cmagicdefense = n5["mdd"];
                    i.cbufftime = n5["time"];
                    i.caccuracy = n5["criticalProb"];
                    i.cavoid = n5["evadeProb"];
                }
    }
}