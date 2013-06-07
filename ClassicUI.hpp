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
#pragma once
namespace NL {
    namespace ClassicUI {
        void Init();
        void Render();
        class Button {
        public:
            Button(Node n, int32_t x, int32_t y);
            void Render(int32_t rel_x, int32_t rel_y);
            enum ButtonState state;
            void SetState(enum ButtonState st) { state = st; }
            uint32_t Width, Height;
        private:
            Sprite sprites[4];
            int32_t x, y;
        };

        enum ButtonState : unsigned char {
            Disabled = 0,
            MouseOver,
            Normal,
            Pressed
        };
        class StatusBar {
        public:
            StatusBar();
            void Render();
            void Load();
            static void Init();
        private:
            void RenderLevel();
            void RenderGauge();
            void RenderButtons();
        };
    }
}