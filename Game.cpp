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
    Node NXBase, NXCharacter, NXEffect, NXEtc, NXItem, NXMap, NXMob, NXMorph, NXNpc, NXQuest, NXReactor, NXSkill, NXSound, NXString, NXTamingMob, NXUI;
    namespace Game {
        bool Over = false;
        void SetupFiles() {
            auto AddFile = [&](char const * c) {
                if (!exists(path(c))) return Node();
                return (new File(c))->Base();
            };
            if (exists(path("Data.nx"))) {
                NXBase = AddFile("Data.nx");
                NXCharacter = NXBase["Character"];
                NXEffect = NXBase["Effect"];
                NXEtc = NXBase["Etc"];
                NXItem = NXBase["Item"];
                NXMap = NXBase["Map"];
                NXMob = NXBase["Mob"];
                NXMorph = NXBase["Morph"];
                NXNpc = NXBase["Npc"];
                NXQuest = NXBase["Quest"];
                NXReactor = NXBase["Reactor"];
                NXSkill = NXBase["Skill"];
                NXSound = NXBase["Sound"];
                NXString = NXBase["String"];
                NXTamingMob = NXBase["TamingMob"];
                NXUI = NXBase["UI"];
            } else if (exists(path("Base.nx"))) {
                NXBase = AddFile("Base.nx");
                NXCharacter = AddFile("Character.nx");
                NXEffect = AddFile("Effect.nx");
                NXEtc = AddFile("Etc.nx");
                NXItem = AddFile("Item.nx");
                NXMap = AddFile("Map.nx");
                NXMob = AddFile("Mob.nx");
                NXMorph = AddFile("Morph.nx");
                NXNpc = AddFile("Npc.nx");
                NXQuest = AddFile("Quest.nx");
                NXReactor = AddFile("Reactor.nx");
                NXSkill = AddFile("Skill.nx");
                NXSound = AddFile("Sound.nx");
                NXString = AddFile("String.nx");
                NXTamingMob = AddFile("TamingMob.nx");
                NXUI = AddFile("UI.nx");
            } else {
                Log::Write("Failed to load data files");
                throw;
            }
        }
        void Play() {
            SetupFiles();
            Time::Init();
            Graphics::Init();
            Map::Init();
            while (!Over) {
                Time::Update();
                Player::Update();
                Graphics::Update();
                Map::Render();
            }
        }
    }
}