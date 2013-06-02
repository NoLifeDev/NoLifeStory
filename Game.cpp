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
    namespace Dumper {
        map<pair<string, string>, size_t> values;
            string pad(string s, size_t n) {
                return move(s).append(n - s.size(), ' ');
            }
            void push_combine(Node n, string s) {
                string ss(move(s) + "/" + n.Name());
                values[make_pair(ss, n.GetString())]++;
                for (Node nn : n) push_combine(nn, ss);
            }
            void push (Node n) {
                values[make_pair(n.Name(), n.GetString())]++;
                for (Node nn : n) push_combine(nn, n.Name());
            }
            void display(string s) {
                size_t l1 = 0, l2 = 0, l3 = 0;
                for (auto i : values) {
                    l1 = max(l1, to_string(i.second).size());
                    l2 = max(l2, i.first.first.size());
                    l3 = max(l3, i.first.second.size());
                }
                size_t l4 = max(s.size(), l1 + l2 + l3 + 6);
                Log::Write(string(l4, '='));
                Log::Write(s);
                Log::Write(string(l4, '='));
                for (auto i : values) Log::Write("[" + pad(to_string(i.second), l1) + "] " + pad(i.first.first, l2) + " = " + i.first.second);
                values.clear();
            }
    }
    namespace Game {
        bool Over = false;
        vector<shared_ptr<File>> Files;
        void SetupFiles() {
            auto AddFile = [&](char const * c) -> Node {
                if (!exists(path(c))) return Node();
                Files.emplace_back(make_shared<File>(c));
                return Files.back()->Base();
            };
            if (exists(path("Base.nx"))) {
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
            } else if (exists(path("Data.nx"))) {
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
                View::Update();
                Map::Render();
                Graphics::Update();
            }
            Graphics::Unload();
        }
    }
}