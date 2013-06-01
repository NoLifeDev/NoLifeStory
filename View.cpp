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
            if (Map::Current["info"]["VRTop"]) {
                Top = Map::Current["info"]["VRTop"];
                Bottom = Map::Current["info"]["VRBottom"];
                Left = Map::Current["info"]["VRLeft"];
                Right = Map::Current["info"]["VRRight"];
                if (Bottom - Top < 600) {
                    int32_t d = (600 - (Bottom - Top)) / 2;
                    Bottom += d;
                    Top -= d;
                }
                if (Right - Left < 800) {
                    int32_t d = (800 - (Right - Left)) / 2;
                    Right += d;
                    Left -= d;
                }
            } else {
                Left = INT32_MAX, Right = INT32_MIN;
                Top = INT32_MAX, Bottom = INT32_MIN;
                for (auto && f : Footholds) {
                    if (Left > f.x1) Left = f.x1;
                    if (Left > f.x2) Left = f.x2;
                    if (Right < f.x1) Right = f.x1;
                    if (Right < f.x2) Right = f.x2;
                    if (Top > f.y1) Top = f.y1;
                    if (Top > f.y2) Top = f.y2;
                    if (Bottom < f.y1) Bottom = f.y1;
                    if (Bottom < f.y2) Bottom = f.y2;
                }
                Top -= 256;
                Bottom += 128;
                if (Top > Bottom - 600) Top = Bottom - 600;
            }
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
        }
        void DrawEdges() {
            Sprite::LoseBind();
            glBindTexture(GL_TEXTURE_2D, 0);
            glColor4f(0, 0, 0, 1);
            glBegin(GL_QUADS);

            glVertex2i(0, 0);
            glVertex2i(Width, 0);
            glVertex2i(Width, Top - Y + Height / 2);
            glVertex2i(0, Top - Y + Height / 2);

            glVertex2i(0, Bottom - Y + Height / 2);
            glVertex2i(Width, Bottom - Y + Height / 2);
            glVertex2i(Width, Height);
            glVertex2i(0, Height);

            glVertex2i(0, Top - Y + Height / 2);
            glVertex2i(Left - X + Width / 2, Top - Y + Height / 2);
            glVertex2i(Left - X + Width / 2, Bottom - Y + Height / 2);
            glVertex2i(0, Bottom - Y + Height / 2);

            glVertex2i(Right - X + Width / 2, Top - Y + Height / 2);
            glVertex2i(Width, Top - Y + Height / 2);
            glVertex2i(Width, Bottom - Y + Height / 2);
            glVertex2i(Right - X + Width / 2, Bottom - Y + Height / 2);

            glEnd();
        }
    }
}