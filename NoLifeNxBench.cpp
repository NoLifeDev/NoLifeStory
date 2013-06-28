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
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstddef>
#ifdef NL_WINDOWS
#  include <Windows.h>
#else
#  include <ctime>
#endif
char const Name[] = "Data.nx";
NL::File const File {Name};
void Load() {
    NL::File f {Name};
}
void SubRecurse(NL::Node n) {
    if (true) for (NL::Node nn : n) SubRecurse(nn); else throw;
}
void LoadRecurse() {
    SubRecurse(NL::File(Name));
}
void Recurse() {
    SubRecurse(File);
}
void SubRecurseSearch(NL::Node n) {
    for (NL::Node nn : n) n[nn.NameFast()] == nn ? SubRecurseSearch(nn) : throw;
}
void RecurseSearch() {
    SubRecurseSearch(File);
}
void SubDecompress(NL::Node n) {
    n.GetBitmap().Data();
    for (NL::Node nn : n) SubDecompress(nn);
}
void Decompress() {
    SubDecompress(File);
}
#ifdef NL_WINDOWS
double Freq;
double GetHPC() {
    LARGE_INTEGER n;
    QueryPerformanceCounter(&n);
    return n.QuadPart * Freq;
}
void SetupTimers() {
    LARGE_INTEGER n;
    QueryPerformanceFrequency(&n);
    Freq = 1000000. / n.QuadPart;
}
#else
int64_t GetHPC() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return 1000000. * t.tv_sec + 0.001 * t.tv_nsec;
}
void SetupTimers() {}
#endif
template <typename T>
void Test(const char * name, T f, size_t maxruns) {
    f();
    std::vector<double> results {};/*
    double c0 {GetHPC()};*/
    do {
        double const c1 {GetHPC()};
        f();
        double const c2 {GetHPC()};
        results.emplace_back(c2 - c1);
    } while (--maxruns/* && GetHPC() - c0 < 2000000*/);
    std::sort(results.begin(), results.end());
    std::vector<double>::const_iterator n0 {results.cbegin() + static_cast<ptrdiff_t>(results.size()) / 4};
    std::vector<double>::const_iterator n1 {results.cbegin() + static_cast<ptrdiff_t>(results.size()) * 3 / 4};
    std::vector<double>::const_iterator n2 {n0 == n1 ? n1 + 1 : n1};
    std::printf("%s\t%llu\t%llu\t%llu\n", name, static_cast<size_t>(*n1), static_cast<size_t>(std::accumulate(n0, n2, 0.) / (n2 - n0)), static_cast<size_t>(results.front()));
}
int main() {
    SetupTimers();
    std::printf("Name\t75%%t\tM50%%\tBest\n");
    Test("Ld", Load, 0x1000);
    Test("Re", Recurse, 0x100);
    Test("LR", LoadRecurse, 0x100);
    Test("SA", RecurseSearch, 0x100);
    Test("De", Decompress, 0x10);
}