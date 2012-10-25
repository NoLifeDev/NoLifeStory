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
#include "NX.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
size_t ticks() {
    LARGE_INTEGER blah;
    QueryPerformanceCounter(&blah);
    return blah.QuadPart;
}
size_t res() {
    LARGE_INTEGER blah;
    QueryPerformanceFrequency(&blah);
    return blah.QuadPart;
}
#else
//define ticks and res using some sort of clock which provides nanosecond resolution
#endif
#include <iostream>
#include <iomanip>

const size_t N = 0x20;
struct Result {
    size_t Count;
    size_t Total;
} Table[0x1000];
void Recurse(NL::Node n) {
    if (!n.Num()) return;
    const size_t last = ticks();
    for (NL::Node nn : n) {
        const NL::String s = nn.Name();
        for (size_t i = N; i; --i) {
            if (n[s] != nn) throw;
        }
    }
    const size_t now  = ticks();
    Table[n.Num()].Count++;
    Table[n.Num()].Total += now - last;
    for (NL::Node nn : n) Recurse(nn);
}


int main() {
    const NL::File file("Data.nx");
    Recurse(file.Base());
    const double div = static_cast<double>(res()) / 1000000000;
    for (size_t i = 0xfff; i; --i) if (Table[i].Count) std::cout << std::setw(4) << i << ": " << std::setw(3) << static_cast<size_t>(Table[i].Total / (div * Table[i].Count * i * N)) << "ns" << std::endl;
}
