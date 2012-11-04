//////////////////////////////////////////////////////////////////////////
// Copyright 2012 Peter Atechian (Retep998)                             //
//////////////////////////////////////////////////////////////////////////
// This file is part of the NoLifeStory project.                        //
//                                                                      //
// NoLifeStory is free software: you can redistribute it and/or modify  //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// NoLifeStory is distributed in the hope that it will be useful,       //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with NoLifeStory.  If not, see <http://www.gnu.org/licenses/>. //
//////////////////////////////////////////////////////////////////////////
#include "NX.hpp"
#include <iostream>
#include <ctime>

NL::File const file("Data.nx");
size_t c;

#define CONSTREF 0
#define RANGEFOR 0
#if CONSTREF
void Recurse(NL::Node const& n) {
#else
void Recurse(NL::Node n) {
#endif
    c++;
#if RANGEFOR
    for (auto nn : n) Recurse(nn);
#else
    auto nn = n.begin();
    for (size_t i = n.Num(); i; --i, ++nn) Recurse(nn);
#endif
}

template <typename T>
void test(T f) {
    size_t best = -1;
    for (;;) {
        c = 0;
        size_t t1 = clock();
        f(file.Base());
        size_t t2 = clock();
        size_t d = t2 - t1;
        if (d < best) {
            best = d;
            std::cout << c << " nodes in " << d * 1000 / CLOCKS_PER_SEC << " ms" << std::endl;
        }
    }
}

int main() {
    test(Recurse);
}
