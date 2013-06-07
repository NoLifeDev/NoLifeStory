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
    namespace ClassicUI {
        StatusBar bar;

        void Init() {
            StatusBar::Init();
        }

        void Render() {
            bar.Render();
        }
        Sprite backgrnd, backgrnd2;
        Sprite level_no[10];
        Sprite gauge_bar, gauge_grad;
        Button::Button(Node n, int32_t x = 0, int32_t y = 0) : state(Normal), x(x), y(y) {
            sprites[Disabled] = n["disabled"];
            sprites[Normal] = n["normal"];
            sprites[MouseOver] = n["mouseOver"];
            sprites[Pressed] = n["pressed"];

            Width = sprites[Normal].Width;
            Height = sprites[Normal].Height;
        }

        void Button::Render(int32_t rel_x, int32_t rel_y) {
            sprites[state].Draw(rel_x + x, rel_y + y, false, false);
        }
        vector<Button> buttons;

        StatusBar::StatusBar() {}// eventually load everything here}

        void StatusBar::Init() {
            backgrnd = NXUI["StatusBar.img"]["base"]["backgrnd"];
            backgrnd2 = NXUI["StatusBar.img"]["base"]["backgrnd2"];
            gauge_bar = NXUI["StatusBar.img"]["gauge"]["bar"];
            gauge_grad = NXUI["StatusBar.img"]["gauge"]["graduation"];

            // Load level numbers
            Node levels = NXUI["Basic.img"]["LevelNo"];
            for (uint8_t i = 0; i < 10; ++i) {
                level_no[i] = levels[i];
            }

            buttons.push_back(Button(NXUI["StatusBar.img"]["BtMenu"], 0, 0));
            buttons.push_back(Button(NXUI["StatusBar.img"]["BtShop"], 0, 0));
            buttons.push_back(Button(NXUI["StatusBar.img"]["BtShort"], 0, 0));
        }

        void StatusBar::Render() {
            backgrnd.Draw(0, View::Height - backgrnd.Height, false, false, true);
            backgrnd2.Draw(0, View::Height - backgrnd.Height, false, false);
            RenderLevel();
            RenderGauge();
            RenderButtons();
        }

        void StatusBar::RenderLevel() {
            string level = to_string(Player::Level);
            int32_t x = 0;
            for (char c : level) level_no[c - 0x30].Draw(30 + (x += 12), View::Height - 24, false, false);
        }

        void StatusBar::RenderGauge() {
            gauge_bar.Draw(215, View::Height - 5 - gauge_bar.Height, false, false);
        }

        void StatusBar::RenderButtons() {
            uint32_t offset = 580;

            for (auto && b : buttons) {
                b.Render(offset, View::Height - b.Height);
                offset += b.Width + 10;
            }
        }
    }
}