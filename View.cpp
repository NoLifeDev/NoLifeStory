//////////////////////////////////////////////////////////////////////////////
// NoLifeClient - Part of the NoLifeStory project                           //
// Copyright (C) 2013 Peter Atashian                             (0)           //
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
    namespace View {
        int32_t X(0), Y(0);
        int32_t Width(0), Height(0);
        double FX(0), FY(0);
        int32_t Left(0), Right(0), Top(0), Bottom(0);
        void Resize(int32_t w, int32_t h) {
            Width = w, Height = h;
            glViewport(0, 0, Width, Height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, Width, Height, 0, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
        template <typename T>
        void Restrict(T & x, T & y) {
            x = Right - Left <= Width ? (Right + Left) / 2 : x > Right - Width / 2 ? Right - Width / 2 : x < Left + Width / 2 ? Left + Width / 2 : x;
            y = Bottom - Top <= Height ? (Bottom + Top) / 2 : y > Bottom - Height / 2 ? Bottom - Height / 2 : y < Top + Height / 2 ? Top + Height / 2 : y;
        }
        void Reset() {
            Left = -1000, Right = 1000;
            Top = -1000, Bottom = 1000;
            X = Player::X, Y = Player::Y;
            Restrict(X, Y);
            FX = X, FY = Y;
        }
        void Update() {
            Restrict(Player::X, Player::Y);//Temporary until player physics implemented
            int32_t tx(Player::X);
            int32_t ty(Player::Y);
            Restrict(tx, ty);
            double sx((tx - X) * Time::Delta * 0.02);
            double sy((ty - Y) * Time::Delta * 0.02);
            FX += sx;
            FY += sy;
            X = FX;
            Y = FY;
            Log::Write(to_string(tx) + ", " + to_string(sx) + ", " + to_string(FX) + ", " + to_string(X));
        }
    }
}