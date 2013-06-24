//////////////////////////////////////////////////////////////////////////////
// NoLifeNxBench - Part of the NoLifeStory project                          //
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
#include "../NoLifeNx/NX.hpp"
#include <cstdio>
#include <cmath>
#include <ctime>
#include <limits>
#ifdef NL_WINDOWS
#  include <Windows.h>
#endif
char const Name[] = "Data.nx";
NL::File const File(Name);
int64_t Freq;
void Load() {
    NL::File f(Name);
}
void SubRecurse(NL::Node n) {
    for (NL::Node nn : n) SubRecurse(nn);
}
void LoadRecurse() {
    SubRecurse(NL::File(Name));
}
void Recurse() {
    SubRecurse(File);
}
void SubRecurseSearch(NL::Node n) {
    for (NL::Node nn : n) if (n[nn.NameFast()] != nn) throw;
    for (NL::Node nn : n) SubRecurseSearch(nn);
}
void RecurseSearch() {
    SubRecurseSearch(File);
}
#ifdef NL_WINDOWS
int64_t GetHPC() {
    LARGE_INTEGER n;
    QueryPerformanceCounter(&n);
    return n.QuadPart;
}
void GetFreq() {
    LARGE_INTEGER n;
    QueryPerformanceFrequency(&n);
    Freq = n.QuadPart;
}
#else
int64_t GetHPC() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return t.tv_sec * 1000000000LL + t.tv_nsec;
}
void GetFreq() {
    Freq = 1000000000LL;
}
#endif
template <typename T>
void Test(const char * name, T f) {
    int64_t best = std::numeric_limits<int64_t>::max();
    int64_t c0 = GetHPC();
    do {
        int64_t c1 = GetHPC();
        f();
        int64_t c2 = GetHPC();
        int64_t dif = c2 - c1;
        if (dif < best) best = dif;
    } while (GetHPC() - c0 < Freq << 2);
    printf("%s: %lldus\n", name, best * 1000000LL / Freq);
}
int main() {
    GetFreq();
    Test("Load", Load);
    Test("Load + Recurse", LoadRecurse);
    Test("Recurse", Recurse);
    Test("Recurse + Search", RecurseSearch);
}