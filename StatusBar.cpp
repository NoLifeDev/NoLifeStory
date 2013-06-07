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
    namespace UI {
        NL::Sprite backgrnd, backgrnd2;
        NL::Sprite level_no[10];
        NL::Sprite gauge_bar, gauge_grad;

        vector<Button> buttons;

        StatusBar::StatusBar() {}// eventually load everything here}

        void StatusBar::Init() {
            backgrnd = NXUI["StatusBar.img"]["base"]["backgrnd"];
            backgrnd2 = NXUI["StatusBar.img"]["base"]["backgrnd2"];
            gauge_bar = NXUI["StatusBar.img"]["gauge"]["bar"];
            gauge_grad = NXUI["StatusBar.img"]["gauge"]["graduation"];

            // Load level numbers
            NL::Node levels = NXUI["Basic.img"]["LevelNo"];
            for (unsigned char i = 0; i < 10; ++i) {
                level_no[i] = levels[to_string(i)];
            }

            buttons.push_back(Button(NXUI["StatusBar.img"]["BtMenu"], 0, 0));
            buttons.push_back(Button(NXUI["StatusBar.img"]["BtShop"], 0, 0));
            buttons.push_back(Button(NXUI["StatusBar.img"]["BtShort"], 0, 0));
        }

        void StatusBar::Render() {
            unsigned char cnt = (View::Width / backgrnd.Width) + 1;
            for (unsigned char i = 0; i < cnt; ++i) {
                backgrnd.Draw(i * backgrnd.Width, View::Height - backgrnd.Height, false, false);
            }
            backgrnd2.Draw(0, View::Height - backgrnd.Height, false, false);

            RenderLevel();
            RenderGauge();
            RenderButtons();
        }

        void StatusBar::RenderLevel() {
            unsigned char level = 69;

            for (unsigned char i = 0; i < 3; ++i) {
                unsigned char n = level % 10;
                level /= 10;
                level_no[n].Draw(60 - i*12, View::Height - 24, false, false);
            }
        }

        void StatusBar::RenderGauge() {
            gauge_bar.Draw(215, View::Height - 5 - gauge_bar.Height, false, false);
        }

        void StatusBar::RenderButtons() {
            unsigned int offset = 580;

            for (auto && b : buttons) {
                b.Render(offset, View::Height - b.Height);
                offset += b.Width + 10;
            }
        }
    }
}