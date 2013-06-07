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
        bool flying = Map::Current["info"]["swim"].GetBool() || true;
        if (flying) {
            vy = -ShoeSwimSpeedV * Wat3;
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

        } else {
            bool flying = Map::Current["info"]["swim"].GetBool() || true;
            if (flying) {
                double vmid = ShoeSwimAcc;
                double vmax = ShoeSwimSpeedH*SwimSpeed;
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
                //         if (vy > 0.) {
                // vy = max(0., vy-floatDrag2/shoe::mass*Time::delta);
                //} else {
                // vy = min(0., vy+floatDrag2/shoe::mass*Time::delta);
                //}
                //vy += gravityAcc*Time::delta;
                //vy = min(vy, fallSpeed);
                //if (moving) {
                // double l = floatDrag2*wat1;
                // if (left) {
                //  if (vx > -l) {
                //   vx = max(-l, vx-floatDrag2*2/shoe::mass*Time::delta);
                //  }
                // } else {
                //  if (vx < l) {
                //   vx = min(l, vx+floatDrag2*2/shoe::mass*Time::delta);
                //  }
                // }
                //} else {
                // if (vy < fallSpeed) {
                //  if (vx > 0) {
                //   vx = max(0., vx-floatDrag2*floatCoefficient/shoe::mass*Time::delta);
                //  } else {
                //   vx = min(0., vx+floatDrag2*floatCoefficient/shoe::mass*Time::delta);
                //  }
                // } else {
                //  if (vx > 0) {
                //   vx = max(0., vx-floatDrag2/shoe::mass*Time::delta);
                //  } else {
                //   vx = min(0., vx+floatDrag2/shoe::mass*Time::delta);
                //  }
                // }
                //}
            }
            x += vx * Time::Delta;
            y += vy * Time::Delta;
            //View culling
            if (x < View::Left + 20) x = View::Left + 20, vx = 0;
            if (x > View::Right - 20) x = View::Right - 20, vx = 0;
            if (y < View::Top - 60) y = View::Top - 60, vy = 0;
            if (y > View::Bottom) y = View::Bottom, vy = 0;
        }
    }
}