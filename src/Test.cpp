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
size_t c = 0;
size_t s;
size_t b;
void Recurse(NL::Node n) {
    if (n.T() == NL::Node::audio) {
        ++c;
        NL::Audio a = n;
        uint8_t const * p = reinterpret_cast<uint8_t const *>(a.Data());
        size_t h;
        b += a.Length();
        for (size_t i = 0; i < a.Length(); ++i) {
            h += p[i];
        }
        s += h;
    }
    for (auto nn : n) Recurse(nn);
}

template <typename T>
void test(T f) {
    size_t t1 = clock();
    f(file.Base());
    size_t t2 = clock();
    std::cout << b << " bytes in " << c << " sounds in " << (t2 - t1) * 1000 / CLOCKS_PER_SEC << " ms" << std::endl;
}

int main() {
    test(Recurse);
}
