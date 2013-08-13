//////////////////////////////////////////////////////////////////////////////
// NoLifeNx - Part of the NoLifeStory project                               //
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
#include "nx.hpp"
#include "file.hpp"
#include "node.hpp"
#include <fstream>
#include <vector>
#include <memory>
#include <stdexcept>
namespace nl {
    namespace {
        std::vector<std::unique_ptr<file>> files {};
        bool exists(std::string name) {
            return std::ifstream {name}.is_open();
        }
        node add_file(std::string name) {
            if (!exists(name)) return {};
            files.emplace_back(new file(name));
            return *files.back();
        }
    }
    namespace nx {
        extern node base, character, effect, etc, item, map, mob, morph, npc, quest, reactor, skill, sound, string, tamingmob, ui;
        void load_all() {
            if (exists("Base.nx")) {
                nx::base = add_file("Base.nx");
                nx::character = add_file("Character.nx");
                nx::effect = add_file("Effect.nx");
                nx::etc = add_file("Etc.nx");
                nx::item = add_file("Item.nx");
                nx::map = add_file("Map.nx");
                nx::mob = add_file("Mob.nx");
                nx::morph = add_file("Morph.nx");
                nx::npc = add_file("Npc.nx");
                nx::quest = add_file("Quest.nx");
                nx::reactor = add_file("Reactor.nx");
                nx::skill = add_file("Skill.nx");
                nx::sound = add_file("Sound.nx");
                nx::string = add_file("String.nx");
                nx::tamingmob = add_file("TamingMob.nx");
                nx::ui = add_file("UI.nx");
            } else if (exists("Data.nx")) {
                nx::base = add_file("Data.nx");
                nx::character = nx::base["Character"];
                nx::effect = nx::base["Effect"];
                nx::etc = nx::base["Etc"];
                nx::item = nx::base["Item"];
                nx::map = nx::base["Map"];
                nx::mob = nx::base["Mob"];
                nx::morph = nx::base["Morph"];
                nx::npc = nx::base["Npc"];
                nx::quest = nx::base["Quest"];
                nx::reactor = nx::base["Reactor"];
                nx::skill = nx::base["Skill"];
                nx::sound = nx::base["Sound"];
                nx::string = nx::base["String"];
                nx::tamingmob = nx::base["TamingMob"];
                nx::ui = nx::base["UI"];
            } else {
                throw std::runtime_error {"Failed to locate nx files."};
            }
        }
    }
}
