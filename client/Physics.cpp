//////////////////////////////////////////////////////////////////////////////
// NoLifeClient - Part of the NoLifeStory project                           //
// Copyright © 2013 Peter Atashian                                          //
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
    double const DownJumpMultiplier = 0.35355339;
    double const Epsilon = 0.00001;
    double const FallSpeed = 670;
    double const FloatCoefficient = 0.01;
    double const FloatDrag1 = 100000;
    double const FloatDrag2 = 10000;
    double const FloatMultiplier = 0.0008928571428571428;
    double const FlyForce = 120000;
    double const FlyJumpDec = 0.35;
    double const FlySpeed = 200;
    double const GravityAcc = 2000;
    double const JumpSpeed = 555;
    double const MaxFriction = 2;
    double const MaxLandSpeed = 162.5;
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
    double const SwimJump = 700;
    double const SwimSpeed = 140;
    double const SwimSpeedDec = 0.9;
    double const WalkDrag = 80000;
    double const WalkForce = 140000;
    double const WalkSpeed = 125;
    Physics::Physics() {
        Reset(0, 0);
        left = false, right = false, up = false, down = false;
    }
    void Physics::Reset(double nx, double ny) {
        x = nx, y = ny, r = 0;
        vx = 0, vy = 0, vr = 0;
        layer = 7, group = -1;
        fh = nullptr, lr = nullptr, djump = nullptr;
        laststep = Time::TDelta;
    }
    void Physics::Jump() {
        bool flying = Map::Current["info"]["swim"].GetBool();
        if (fh) {
            if (down && !fh->cantThrough && !fh->forbidFallDown && any_of(Footholds.begin(), Footholds.end(), [&](Foothold & f) {
                return f.id != fh->id && f.x1 < x && f.x2 > x && f.y1 > y && f.y2 > y;
            })) {
                djump = fh;
                vx = 0, vy = -JumpSpeed * DownJumpMultiplier;
            } else {
                vy = ShoeWalkJump * JumpSpeed * (flying ? -0.7 : -1);
                double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1, fmax = WalkSpeed * ShoeWalkSpeed;
                (left && fy < 0) || (right && fy > 0) ? fmax *= (1 + (fy * fy) / (fx * fx +  fy * fy)) : 0;
                vx = left ? max(min(vx, -fmax * 0.8), -fmax) : right ? min(max(vx, fmax * 0.8), fmax) : vx;
            }
            fh = nullptr;
        } else {
            if (flying) {
                vy = -ShoeSwimSpeedV * SwimJump;
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
        bool flying = Map::Current["info"]["swim"].GetBool();
        while (Time::TDelta > laststep) {
            laststep += 0.01;
            double delta = 0.01;
            if (lr) {
            } else if (fh) {
                double const fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1, fx2 = fx * fx, fy2 = fy * fy, len = sqrt(fx2 + fy2);
                double vr = vx * len / fx;
                vr -= fh->force;
                double fs = (Map::Current["info"]["fs"] ? Map::Current["info"]["fs"] : 1.) / ShoeMass * delta;
                double maxf = (flying ? SwimSpeedDec : 1.) * WalkSpeed * ShoeWalkSpeed;
                double horz = ShoeWalkAcc * WalkForce;
                double drag = max(min(ShoeWalkDrag, MaxFriction), MinFriction) * WalkDrag;
                double slip = fy / len;
                if (ShoeWalkSlant < abs(slip)) {
                    double slipf = SlipForce * slip;
                    double slips = SlipSpeed * slip;
                    vr += left ? -drag * fs : right ? drag * fs : 0;
                    vr = slips > 0 ? min(slips, vr + slipf * delta) : max(slips, vr + slipf * delta);
                } else {
                    vr = left ? vr < -maxf ? min(-maxf, vr + drag * fs) : max(-maxf, vr - ShoeWalkAcc * WalkForce * fs) :
                        right ? vr > maxf ? max(maxf, vr - drag * fs) : min(maxf, vr + ShoeWalkAcc * WalkForce * fs) :
                        vr < 0. ? min(0., vr + drag * fs) : vr > 0. ? max(0., vr - drag * fs) : vr;
                }
                vr += fh->force;
                vx = vr * fx / len, vy = vr * fy / len;
            } else {
                if (flying) {
                    double vmid = ShoeSwimAcc;
                    double vmax = ShoeSwimSpeedH * SwimSpeed;
                    double shoefloat = FloatDrag1 / ShoeMass * delta;
                    vx = vx < -vmax ? min(-vmax, vx + shoefloat) : vx > vmax ? max(vmax, vx - shoefloat) :
                        left ? max(-vmax, vx - shoefloat) : right ? min(vmax, vx + shoefloat) :
                        vx > 0 ? max(0., vx - shoefloat) : min(0., vx + shoefloat);
                    double flys = FlyForce / ShoeMass * delta * vmid;
                    vy = up ? vy < vmax * 0.3 ? min(vmax * 0.3, vy + flys * 0.5) : max(vmax * 0.3, vy - flys) :
                        down ? vy < vmax * 1.5 ? min(vmax * 1.5, vy + flys) : max(vmax * 1.5, vy - flys * 0.5) :
                        vy < vmax ? min(vmax, vy + flys) : max(vmax, vy - flys);
                } else {
                    double shoefloat = FloatDrag2 / ShoeMass * delta;
                    vy > 0 ? vy = max(0., vy - shoefloat) : vy = min(0., vy + shoefloat);
                    vy = min(vy + GravityAcc * delta, FallSpeed);
                    vx = left ? vx > -FloatDrag2 * FloatMultiplier ? max(-FloatDrag2 * FloatMultiplier, vx - 2 * shoefloat) : vx :
                        right ? vx < FloatDrag2 * FloatMultiplier ? min(FloatDrag2 * FloatMultiplier, vx + 2 * shoefloat) : vx :
                        vy < FallSpeed ? vx > 0 ? max(0., vx - FloatCoefficient * shoefloat) : min(0., vx + FloatCoefficient * shoefloat) :
                        vx > 0 ? max(0., vx - shoefloat) : min(0., vx + shoefloat);
                }
            }
            while (delta > Epsilon) {
                if (lr) {

                } else if (fh) {
                    double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1;
                    double nx = x + vx * delta, ny = y + vy * delta;
                    if (nx > View::Right - 20) {
                        nx = View::Right - 20 - Epsilon;
                        ny = fh->y1 + (nx - fh->x1) * fy / fx;
                        vx = 0, vy = 0;
                        delta = 0;
                    } else if (nx < View::Left + 20) {
                        nx = View::Left + 20 + Epsilon;
                        ny = fh->y1 + (nx - fh->x1) * fy / fx;
                        vx = 0, vy = 0;
                        delta = 0;
                    } else if (nx > fh->x2) {
                        if (!fh->next) {
                            nx = fh->x2 + Epsilon, ny = fh->y2;
                            fh = nullptr;
                            delta *= 1 - (nx - x) / (vx * delta);
                        } else if (fh->next->x1 < fh->next->x2) {
                            fh = fh->next;
                            double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1;
                            double dot = (vx * fx + vy * fy) / (fx * fx + fy * fy);
                            nx = fh->x1, ny = fh->y1;
                            delta *= 1 - (nx - x) / (vx * delta);
                            vx = dot * fx, vy = dot * fy;
                        } else if (fh->next->y1 > fh->next->y2) {
                            nx = fh->x2 - Epsilon, ny = fh->y2;
                            vx = 0, vy = 0;
                            delta = 0;
                        } else {
                            nx = fh->x2 + Epsilon, ny = fh->y2;
                            fh = nullptr;
                            delta *= 1 - (nx - x) / (vx * delta);
                        }
                    } else if (nx < fh->x1) {
                        if (!fh->prev) {
                            nx = fh->x1 - Epsilon, ny = fh->y1;
                            fh = nullptr;
                            delta *= 1 - (nx - x) / (vx * delta);
                        } else if (fh->prev->x1 < fh->prev->x2) {
                            fh = fh->prev;
                            double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1;
                            double dot = (vx * fx + vy * fy) / (fx * fx + fy * fy);
                            nx = fh->x2, ny = fh->y2;
                            delta *= 1 - (nx - x) / (vx * delta);
                            vx = dot * fx, vy = dot * fy;
                        } else if (fh->prev->y1 < fh->prev->y2) {
                            nx = fh->x1 + Epsilon, ny = fh->y1;
                            vx = 0, vy = 0;
                            delta = 0;
                        } else {
                            nx = fh->x1 - Epsilon, ny = fh->y1;
                            fh = nullptr;
                            delta *= 1 - (nx - x) / (vx * delta);
                        }
                    } else {
                        delta = 0;
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
                        if (n1 >= 0 && n1 <= 1 && n2 >= 0 && denom < 0 && &f != djump && n2 <= distance)
                            if (group == f.group || dx2 > 0 || f.group == 0 || f.cantThrough) {
                                nnx = x + n2 * dx1, nny = y + n2 * dy1;
                                distance = n2;
                                fh = &f;
                            }
                    }
                    x = nnx, y = nny;
                    if (fh) {
                        djump = nullptr;
                        double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1;
                        if (fh->x1 > fh->x2) {
                            y += Epsilon;
                            fh = nullptr;
                        } else if (fh->x1 == fh->x2) {
                            if (fy > 0) x += Epsilon;
                            else x -= Epsilon;
                            fh = nullptr;
                        } else {
                            group = fh->group, layer = fh->layer;
                            if (vy > MaxLandSpeed) vy = MaxLandSpeed;
                        }
                        double dot = (vx * fx + vy * fy) / (fx * fx + fy * fy);
                        vx = dot * fx, vy = dot * fy;
                        delta *= 1 - distance;
                    } else {
                        delta = 0;
                    }
                }//} else {
            }//while (delta > Epsilon) {
        }//while (Time::TDelta > laststep) {
    }//void Physics::Update() {
}//namespace NL {
