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
    double const down_jump_multiplier = 0.35355339;
    double const epsilon = 0.00001;
    double const fall_speed = 670;
    double const float_coefficient = 0.01;
    double const float_drag_1 = 100000;
    double const float_drag_2 = 10000;
    double const float_multiplier = 0.0008928571428571428;
    double const fly_force = 120000;
    double const fly_jump_dec = 0.35;
    double const fly_speed = 200;
    double const gravity_acc = 2000;
    double const jump_speed = 555;
    double const max_friction = 2;
    double const max_land_speed = 162.5;
    double const min_friction = 0.05;
    double const shoe_fly_acc = 0;
    double const shoe_fly_speed = 0;
    double const shoe_mass = 100;
    double const shoe_swim_acc = 1;
    double const shoe_swim_speed_h = 1;
    double const shoe_swim_speed_v = 1;
    double const shoe_walk_acc = 1;
    double const shoe_walk_drag = 1;
    double const shoe_walk_jump = 1.2;
    double const shoe_walk_slant = 0.9;
    double const shoe_walk_speed = 1.4;
    double const slip_force = 60000;
    double const slip_speed = 120;
    double const swim_force = 120000;
    double const swim_jump = 700;
    double const swim_speed = 140;
    double const swim_speed_dec = 0.9;
    double const walk_drag = 80000;
    double const walk_force = 140000;
    double const walk_speed = 125;
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
                vx = 0, vy = -jump_speed * down_jump_multiplier;
            } else {
                vy = shoe_walk_jump * jump_speed * (flying ? -0.7 : -1);
                double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1, fmax = walk_speed * shoe_walk_speed;
                (left && fy < 0) || (right && fy > 0) ? fmax *= (1 + (fy * fy) / (fx * fx +  fy * fy)) : 0;
                vx = left ? max(min(vx, -fmax * 0.8), -fmax) : right ? min(max(vx, fmax * 0.8), fmax) : vx;
            }
            fh = nullptr;
        } else {
            if (flying) {
                vy = -shoe_swim_speed_v * swim_jump;
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
                double fs = (Map::Current["info"]["fs"] ? Map::Current["info"]["fs"] : 1.) / shoe_mass * delta;
                double maxf = (flying ? swim_speed_dec : 1.) * walk_speed * shoe_walk_speed;
                double horz = shoe_walk_acc * walk_force;
                double drag = max(min(shoe_walk_drag, max_friction), min_friction) * walk_drag;
                double slip = fy / len;
                if (shoe_walk_slant < abs(slip)) {
                    double slipf = slip_force * slip;
                    double slips = slip_speed * slip;
                    vr += left ? -drag * fs : right ? drag * fs : 0;
                    vr = slips > 0 ? min(slips, vr + slipf * delta) : max(slips, vr + slipf * delta);
                } else {
                    vr = left ? vr < -maxf ? min(-maxf, vr + drag * fs) : max(-maxf, vr - shoe_walk_acc * walk_force * fs) :
                        right ? vr > maxf ? max(maxf, vr - drag * fs) : min(maxf, vr + shoe_walk_acc * walk_force * fs) :
                        vr < 0. ? min(0., vr + drag * fs) : vr > 0. ? max(0., vr - drag * fs) : vr;
                }
                vr += fh->force;
                vx = vr * fx / len, vy = vr * fy / len;
            } else {
                if (flying) {
                    double vmid = shoe_swim_acc;
                    double vmax = shoe_swim_speed_h * swim_speed;
                    double shoefloat = float_drag_1 / shoe_mass * delta;
                    vx = vx < -vmax ? min(-vmax, vx + shoefloat) : vx > vmax ? max(vmax, vx - shoefloat) :
                        left ? max(-vmax, vx - shoefloat) : right ? min(vmax, vx + shoefloat) :
                        vx > 0 ? max(0., vx - shoefloat) : min(0., vx + shoefloat);
                    double flys = fly_force / shoe_mass * delta * vmid;
                    vy = up ? vy < vmax * 0.3 ? min(vmax * 0.3, vy + flys * 0.5) : max(vmax * 0.3, vy - flys) :
                        down ? vy < vmax * 1.5 ? min(vmax * 1.5, vy + flys) : max(vmax * 1.5, vy - flys * 0.5) :
                        vy < vmax ? min(vmax, vy + flys) : max(vmax, vy - flys);
                } else {
                    double shoefloat = float_drag_2 / shoe_mass * delta;
                    vy > 0 ? vy = max(0., vy - shoefloat) : vy = min(0., vy + shoefloat);
                    vy = min(vy + gravity_acc * delta, fall_speed);
                    vx = left ? vx > -float_drag_2 * float_multiplier ? max(-float_drag_2 * float_multiplier, vx - 2 * shoefloat) : vx :
                        right ? vx < float_drag_2 * float_multiplier ? min(float_drag_2 * float_multiplier, vx + 2 * shoefloat) : vx :
                        vy < fall_speed ? vx > 0 ? max(0., vx - float_coefficient * shoefloat) : min(0., vx + float_coefficient * shoefloat) :
                        vx > 0 ? max(0., vx - shoefloat) : min(0., vx + shoefloat);
                }
            }
            while (delta > epsilon) {
                if (lr) {

                } else if (fh) {
                    double fx = fh->x2 - fh->x1, fy = fh->y2 - fh->y1;
                    double nx = x + vx * delta, ny = y + vy * delta;
                    if (nx > View::Right - 20) {
                        nx = View::Right - 20 - epsilon;
                        ny = fh->y1 + (nx - fh->x1) * fy / fx;
                        vx = 0, vy = 0;
                        delta = 0;
                    } else if (nx < View::Left + 20) {
                        nx = View::Left + 20 + epsilon;
                        ny = fh->y1 + (nx - fh->x1) * fy / fx;
                        vx = 0, vy = 0;
                        delta = 0;
                    } else if (nx > fh->x2) {
                        if (!fh->next) {
                            nx = fh->x2 + epsilon, ny = fh->y2;
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
                            nx = fh->x2 - epsilon, ny = fh->y2;
                            vx = 0, vy = 0;
                            delta = 0;
                        } else {
                            nx = fh->x2 + epsilon, ny = fh->y2;
                            fh = nullptr;
                            delta *= 1 - (nx - x) / (vx * delta);
                        }
                    } else if (nx < fh->x1) {
                        if (!fh->prev) {
                            nx = fh->x1 - epsilon, ny = fh->y1;
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
                            nx = fh->x1 + epsilon, ny = fh->y1;
                            vx = 0, vy = 0;
                            delta = 0;
                        } else {
                            nx = fh->x1 - epsilon, ny = fh->y1;
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
                            y += epsilon;
                            fh = nullptr;
                        } else if (fh->x1 == fh->x2) {
                            if (fy > 0) x += epsilon;
                            else x -= epsilon;
                            fh = nullptr;
                        } else {
                            group = fh->group, layer = fh->layer;
                            if (vy > max_land_speed) vy = max_land_speed;
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
