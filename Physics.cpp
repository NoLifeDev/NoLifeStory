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
    double const FallSpeed = 670;
    double const FloatCoefficient = 0.01;
    double const FloatDrag1 = 100000;
    double const FloatDrag2 = 10000;
    double const FlyForce = 120000;
    double const FlyJumpDec = 0.35;
    double const FlySpeed = 200;
    double const GravityAcc = 2000;
    double const JumpSpeed = 555;
    double const MaxFriction = 2;
    double const MinFriction = 0.05;
    double const ShoeFlyAcc = 0;
    double const ShoeFlySpeed = 0;
    double const ShoeMass = 100;
    double const ShoeSwimAcc = 1;
    double const ShoeSwimSpeedH = 1;
    double const ShoeSwimSpeedV = 1;
    double const ShoeWalkAcc = 1;
    double const ShoeWalkDrag = 1;
    double const ShoeWalkJump = 1.2;
    double const ShoeWalkSlant = 0.9;
    double const ShoeWalkSpeed = 1.4;
    double const SlipForce = 60000;
    double const SlipSpeed = 120;
    double const SwimForce = 120000;
    double const SwimSpeed = 140;
    double const SwimSpeedDec = 0.9;
    double const WalkDrag = 80000;
    double const WalkForce = 140000;
    double const WalkSpeed = 125;
    double const Wat1 = 0.0008928571428571428;
    double const Wat2 = 0.35355339;
    double const Wat3 = 700;
    double const Wat4 = 162.5;
    Physics::Physics() {
        Reset(0, 0);
        left = false, right = false, up = false, down = false;
    }
    void Physics::Reset(double nx, double ny) {
        x = nx, y = ny, r = 0;
        vx = 0, vy = 0, vr = 0;
        layer = 7, group = 0;
        fh = nullptr, lr = nullptr;
    }
    void Physics::Jump() {
        if (fh) {
            vy = -JumpSpeed;
            fh = nullptr;
        } else {
            bool flying = Map::Current["info"]["swim"].GetBool();
            if (flying) {
                vy = -ShoeSwimSpeedV * Wat3;
            }
        }
    }
    void Physics::Update() {
        //First get player input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            auto p = sf::Mouse::getPosition(*Graphics::Window);
            Reset(p.x + View::X - View::Width / 2, p.y + View::Y - View::Height / 2);
        }
        bool left = this->left && !this->right;
        bool right = !this->left && this->right;
        bool up = this->up && !this->down;
        bool down = !this->up && this->down;
        if (lr) {//Do ladderrope stuff
        } else if (fh) {//Do foothold stuff
            double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1;
            double add = (left ? -1000 : right ? 1000 : 0) * Time::Delta;
            double dot = add * fx / (fx * fx + fy * fy);
            vx += fx * dot, vy += fy * dot;
            x += vx * Time::Delta, y += vy * Time::Delta;
            if (x > fh->x2) {
                if (!fh->next) {
                    fh = nullptr;
                } else if (fh->next->x1 < fh->next->x2) {
                    fh = fh->next;
                    double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1;
                    double dot = (vx * fx + vy * fy) / (fx * fx + fy * fy);
                    vx = dot * fx, vy = dot * fy;
                    x = fh->x1, y = fh->y1;
                } else if (fh->next->y1 > fh->next->y2) {
                    x = fh->x1 + 0.99999 * fx, y = fh->y1 + 0.99999 * fy;
                    vx = 0, vy = 0;
                } else {
                    fh = nullptr;
                }
            } else if (x < fh->x1) {
                if (!fh->prev) {
                    fh = nullptr;
                } else if (fh->prev->x1 < fh->prev->x2) {
                    fh = fh->prev;
                    double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1;
                    double dot = (vx * fx + vy * fy) / (fx * fx + fy * fy);
                    vx = dot * fx, vy = dot * fy;
                    x = fh->x2, y = fh->y2;
                } else if (fh->prev->y1 < fh->prev->y2) {
                    x = fh->x1 + 0.00001 * fx, y = fh->y1 + 0.00001 * fy;
                    vx = 0, vy = 0;
                } else {
                    fh = nullptr;
                }
            }
        } else {
            bool flying = Map::Current["info"]["swim"].GetBool();
            if (flying) {
                double vmid = ShoeSwimAcc;
                double vmax = ShoeSwimSpeedH * SwimSpeed;
                double shoefloat = FloatDrag1 / ShoeMass * Time::Delta;
                if (vx < -vmax) vx = min(-vmax, vx + shoefloat);
                else if (vx > vmax) vx = max(vmax, vx - shoefloat);
                else if (left) vx = max(-vmax, vx - shoefloat);
                else if (right) vx = min(vmax, vx + shoefloat);
                else if (vx > 0) vx = max(0., vx - shoefloat);
                else vx = min(0., vx + shoefloat);
                double flys = FlyForce / ShoeMass * Time::Delta * vmid;
                if (up) vy < vmax * 0.3 ? vy = min(vmax * 0.3, vy + flys * 0.5) : vy = max(vmax * 0.3, vy - flys);
                else if (down) vy < vmax * 1.5 ? vy = min(vmax * 1.5, vy + flys) : vy = max(vmax * 1.5, vy - flys * 0.5);
                else if (vy < vmax) vy = min(vmax, vy + flys);
                else vy = max(vmax, vy - flys);
            } else {
                if (vy > 0) max(0., vy - FloatDrag2 / ShoeMass * Time::Delta);
                else min(0., vy + FloatDrag2 / ShoeMass * Time::Delta);
                vy += GravityAcc * Time::Delta;
                vy = min(vy, FallSpeed);
                if (left) vx > -FloatDrag1 * Wat1 ? vx = max(-FloatDrag1 * Wat1, vx - 2 * FloatDrag2 / ShoeMass * Time::Delta) : 0;
                else if (right) vx < FloatDrag1 * Wat1 ? vx = min(FloatDrag1 * Wat1, vx + 2 * FloatDrag2 / ShoeMass * Time::Delta) : 0;
                else if (vy < FallSpeed) vx > 0 ? vx = max(0., vx - FloatDrag2 * FloatCoefficient / ShoeMass * Time::Delta) : 
                    vx = min(0., vx + FloatDrag2 * FloatCoefficient / ShoeMass * Time::Delta);
                else vx > 0 ? vx = max(0., vx - FloatDrag2 / ShoeMass * Time::Delta) : 
                    vx = min(0., vx + FloatDrag2 / ShoeMass * Time::Delta);
            }
            //View culling
            if (x < View::Left + 20) x = View::Left + 20, vx = 0;
            if (x > View::Right - 20) x = View::Right - 20, vx = 0;
            if (y < View::Top - 60) y = View::Top - 60, vy = 0;
            if (y > View::Bottom) y = View::Bottom, vy = 0;
            //Collisions
            double dx1 = vx * Time::Delta, dy1 = vy * Time::Delta;
            double distance = 1;
            double nnx = x + dx1, nny = y + dy1;
            for (Foothold & f : Footholds) {
                double dx2 = f.x2 - f.x1, dy2 = f.y2 - f.y1;
                double dx3 = x - f.x1, dy3 = y - f.y1;
                double denom = dx1 * dy2 - dy1 * dx2;
                double n1 = (dx1 * dy3 - dy1 * dx3) / denom;
                double n2 = (dx2 * dy3 - dy2 * dx3) / denom;
                if (n1 >= 0 && n1 <= 1 && n2 >= 0 && denom < 0 && n2 <= distance) {
                    nnx = x + n2 * dx1, nny = y + n2 * dy1;
                    distance = n2;
                    fh = &f;
                }
            }
            x = nnx, y = nny;
            if (fh) {
                double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1;
                double dot = (vx * fx + vy * fy) / (fx * fx + fy * fy);
                Log::Write("(" + to_string(fx) + ", " + to_string(fy) + ") * " + to_string(dot));
                vx = dot * fx, vy = dot * fy;
                if (fh->x1 > fh->x2) {
                    y += 0.01;
                    fh = nullptr;
                } else if (fh->x1 == fh->x2) {
                    if (fh->y1 < fh->y2) x += 0.01;
                    else x -= 0.01;
                    fh = nullptr;
                }
            }
        }
    }
}