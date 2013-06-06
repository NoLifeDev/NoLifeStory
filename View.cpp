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
        template <typename T>
        void Restrict(T & x, T & y) {
            x = Right - Left <= Width ? (Right + Left) / 2 : x > Right - Width / 2 ? Right - Width / 2 : x < Left + Width / 2 ? Left + Width / 2 : x;
            y = Bottom - Top <= Height ? (Bottom + Top) / 2 : y > Bottom - Height / 2 ? Bottom - Height / 2 : y < Top + Height / 2 ? Top + Height / 2 : y;
        }
        void Resize(int32_t w, int32_t h) {
            Width = w, Height = h;
            glViewport(0, 0, Width, Height);
            if (Config::Fullscreen) Config::FullscreenWidth = Width, Config::FullscreenHeight = Height;
            else Config::WindowWidth = Width, Config::WindowHeight = Height;
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
                Left = numeric_limits<int32_t>::max(), Right = numeric_limits<int32_t>::min();
                Top = numeric_limits<int32_t>::max(), Bottom = numeric_limits<int32_t>::min();
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
            X = Player::Pos.x, Y = Player::Pos.y;
            Restrict(X, Y);
            FX = X, FY = Y;
        }
        void Update() {
            double tx(Player::Pos.x), ty(Player::Pos.y);
            Restrict(tx, ty);
            double sx((tx - FX) * Time::Delta * 10), sy((ty - FY) * Time::Delta * 10);
            if (abs(sx) > abs(tx - FX)) sx = tx - FX;
            if (abs(sy) > abs(ty - FY)) sy = ty - FY;
            FX += sx, FY += sy;
            Restrict(FX, FY);
            X = FX, Y = FY;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            if (Config::Rave) {
                mt19937_64 engine(high_resolution_clock::now().time_since_epoch().count());
                uniform_int_distribution<int> dist(-10, 10);
                X += dist(engine);
                Y += dist(engine);
                gluPerspective(-10 * pow(0.5 * sin(Time::TDelta * 2.088 * 2 * M_PI) + 0.5, 9) + 90, double(Width) / Height, 0.1, 10000);
                gluLookAt(Width / 2, Height / 2, -Height / 2, Width / 2, Height / 2, 0, 0, -1, 0);
            } else glOrtho(0, Width, Height, 0, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
        void DrawEdges() {
            Sprite::Unbind();
            glColor4f(0, 0, 0, 1);
            Graphics::DrawRect(0, 0, Width, Top - Y + Height / 2, false);
            Graphics::DrawRect(0, Bottom - Y + Height / 2, Width, Height, false);
            Graphics::DrawRect(0, Top - Y + Height / 2, Left - X + Width / 2, Bottom - Y + Height / 2, false);
            Graphics::DrawRect(Right - X + Width / 2, Top - Y + Height / 2, Width, Bottom - Y + Height / 2, false);
        }
    }
}