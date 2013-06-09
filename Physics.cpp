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
    double const Epsilon = 0.00001;
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
        layer = 7, group = -1;
        fh = nullptr, lr = nullptr, djump = nullptr;
    }
    void Physics::Jump() {
        bool flying = Map::Current["info"]["swim"].GetBool();
        if (fh) {
            if (down && !fh->cantThrough && !fh->forbidFallDown && any_of(Footholds.begin(), Footholds.end(), [&](Foothold & f) {
                return f.id != fh->id && f.x1 < x && f.x2 > x && f.y1 > y && f.y2 > y;
            })) {
                djump = fh;
                vx = 0, vy = -JumpSpeed * Wat2;
                fh = nullptr;
            } else {
                vy = -JumpSpeed;
                fh = nullptr;
                if (flying) vy *= 0.7;
            }
        } else {
            if (flying) {
                vy = -ShoeSwimSpeedV * Wat3;
            }
        }
    }
    void Physics::Update() {
        //First get player input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            auto p = sf::Mouse::getPosition(*Graphics::Window);
            return Reset(p.x + View::X - View::Width / 2, p.y + View::Y - View::Height / 2);
        }
        bool left = this->left && !this->right;
        bool right = !this->left && this->right;
        bool up = this->up && !this->down;
        bool down = !this->up && this->down;
        double total = Time::Delta;
        while (total > 0) {
            double delta = min(total, 0.01);
            //First compute movement itself such as collisions and such to determine delta
            if (lr) {

            } else if (fh) {
                double nx = x + vx * delta, ny = y + vy * delta;
                if (nx > View::Right - 20) {
                    ny = y + (View::Right - 20 - x) * vy / vx;
                    nx = View::Right - 20 - Epsilon;
                    vx = 0, vy = 0;
                } else if (nx < View::Left + 20) {
                    ny = y + (View::Left + 20 - x) * vy / vx;
                    nx = View::Left + 20 + Epsilon;
                    vx = 0, vy = 0;
                } else if (nx > fh->x2) {
                    if (!fh->next) {
                        nx = fh->x2 + Epsilon, ny = fh->y2;
                        fh = nullptr;
                        delta *= (nx - x) / (vx * delta);
                    } else if (fh->next->x1 < fh->next->x2) {
                        fh = fh->next;
                        double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1;
                        double dot = (vx * fx + vy * fy) / (fx * fx + fy * fy);
                        vx = dot * fx, vy = dot * fy;
                        nx = fh->x1, ny = fh->y1;
                        delta *= (nx - x) / (vx * delta);
                    } else if (fh->next->y1 > fh->next->y2) {
                        nx = fh->x2 - Epsilon, ny = fh->y2;
                        vx = 0, vy = 0;
                    } else {
                        nx = fh->x2 + Epsilon, ny = fh->y2;
                        fh = nullptr;
                        delta *= (nx - x) / (vx * delta);
                    }
                } else if (nx < fh->x1) {
                    if (!fh->prev) {
                        nx = fh->x1 - Epsilon, ny = fh->y1;
                        fh = nullptr;
                        delta *= (nx - x) / (vx * delta);
                    } else if (fh->prev->x1 < fh->prev->x2) {
                        fh = fh->prev;
                        double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1;
                        double dot = (vx * fx + vy * fy) / (fx * fx + fy * fy);
                        vx = dot * fx, vy = dot * fy;
                        nx = fh->x2, ny = fh->y2;
                        delta *= (nx - x) / (vx * delta);
                    } else if (fh->prev->y1 < fh->prev->y2) {
                        nx = fh->x1 + Epsilon, ny = fh->y1;
                        vx = 0, vy = 0;
                    } else {
                        nx = fh->x1 - Epsilon, ny = fh->y1;
                        fh = nullptr;
                        delta *= (nx - x) / (vx * delta);
                    }
                }
                x = nx, y = ny;
            } else {
                double dx1 = vx * delta, dy1 = vy * delta;
                double distance = 1;
                double nnx = x + dx1, nny = y + dy1;
                if (nnx < View::Left + 20) nnx = View::Left + 20, vx = 0;
                if (nnx > View::Right - 20) nnx = View::Right - 20, vx = 0;
                if (nny < View::Top - 60) nny = View::Top - 60, vy = 0;
                if (nny > View::Bottom) nny = View::Bottom, vy = 0;
                for (Foothold & f : Footholds) {
                    double dx2 = f.x2 - f.x1, dy2 = f.y2 - f.y1;
                    double dx3 = x - f.x1, dy3 = y - f.y1;
                    double denom = dx1 * dy2 - dy1 * dx2;
                    double n1 = (dx1 * dy3 - dy1 * dx3) / denom;
                    double n2 = (dx2 * dy3 - dy2 * dx3) / denom;
                    if (n1 >= 0 && n1 <= 1 && n2 >= 0 && denom < 0 && &f != djump && n2 <= distance && (group == f.group || dx2 > 0)) {
                        nnx = x + n2 * dx1, nny = y + n2 * dy1;
                        distance = n2;
                        fh = &f;
                    }
                }
                x = nnx, y = nny;
                if (fh) {
                    djump = nullptr;
                    double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1;
                    double dot = (vx * fx + vy * fy) / (fx * fx + fy * fy);
                    vx = dot * fx, vy = dot * fy;
                    if (fh->x1 > fh->x2) {
                        y += Epsilon;
                        fh = nullptr;
                    } else if (fh->x1 == fh->x2) {
                        if (fh->y1 < fh->y2) x += Epsilon;
                        else x -= Epsilon;
                        fh = nullptr;
                    } else {
                        group = fh->group, layer = fh->layer;
                    }
                    delta *= distance;
                }
            }
            //Then adjust the velocity through friction and such
            if (lr) {

            } else if (fh) {
                double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1;
                double add = (left ? -1000 : right ? 1000 : 0) * delta;
                double dot = add * fx / (fx * fx + fy * fy);
                vx += fx * dot, vy += fy * dot;
            } else {
                bool flying = Map::Current["info"]["swim"].GetBool();
                if (flying) {
                    double vmid = ShoeSwimAcc;
                    double vmax = ShoeSwimSpeedH * SwimSpeed;
                    double shoefloat = FloatDrag1 / ShoeMass * delta;
                    vx < -vmax ? vx = min(-vmax, vx + shoefloat) : vx > vmax ? vx = max(vmax, vx - shoefloat) :
                        left ? vx = max(-vmax, vx - shoefloat) : right ? vx = min(vmax, vx + shoefloat) :
                        vx > 0 ? vx = max(0., vx - shoefloat) : vx = min(0., vx + shoefloat);
                    double flys = FlyForce / ShoeMass * delta * vmid;
                    up ? vy < vmax * 0.3 ? vy = min(vmax * 0.3, vy + flys * 0.5) : vy = max(vmax * 0.3, vy - flys) :
                        down ? vy < vmax * 1.5 ? vy = min(vmax * 1.5, vy + flys) : vy = max(vmax * 1.5, vy - flys * 0.5) :
                        vy < vmax ? vy = min(vmax, vy + flys) : vy = max(vmax, vy - flys);
                } else {
                    double shoefloat = FloatDrag2 / ShoeMass * delta;
                    vy > 0 ? vy = max(0., vy - shoefloat) : vy = min(0., vy + shoefloat);
                    vy = min(vy + GravityAcc * delta, FallSpeed);
                    left ? vx > -FloatDrag2 * Wat1 ? vx = max(-FloatDrag2 * Wat1, vx - 2 * shoefloat) : 0 :
                        right? vx < FloatDrag2 * Wat1 ? vx = min(FloatDrag2 * Wat1, vx + 2 * shoefloat) : 0 :
                        vy < FallSpeed ? vx > 0 ? vx = max(0., vx - FloatCoefficient * shoefloat) : vx = min(0., vx + FloatCoefficient * shoefloat) :
                        vx > 0 ? vx = max(0., vx - shoefloat) : vx = min(0., vx + shoefloat);
                }
            }
            total -= delta;
        }
    }
}