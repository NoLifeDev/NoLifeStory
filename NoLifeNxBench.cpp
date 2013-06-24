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
#ifdef NL_WINDOWS
#  include <Windows.h>
#else
#  include <ctime>
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
int64_t Adjust(int64_t v) {
    return v * 1000000LL / Freq;
}
template <typename T>
void Test(const char * name, T f, size_t maxruns) {
    std::vector<int64_t> results;
    int64_t c0 = GetHPC();
    do {
        int64_t c1 = GetHPC();
        f();
        int64_t c2 = GetHPC();
        results.emplace_back(c2 - c1);
    } while (--maxruns && GetHPC() - c0 < Freq << 4);
    std::sort(results.begin(), results.end());
    auto n0 = results.cbegin() + static_cast<ptrdiff_t>(results.size()) / 4;
    auto n1 = results.cbegin() + static_cast<ptrdiff_t>(results.size()) * 3 / 4;
    auto n2 = n0 == n1 ? n1 + 1 : n1;
    std::printf("%s\t%lld\t%lld\t%lld\n", name, Adjust(*n1), Adjust(std::accumulate(n0, n2, 0) / (n2 - n0)), Adjust(results.front()));
}
int main() {
    GetFreq();
    std::printf("Name\t75%%t\tM50%%\tBest\n");
    Test("Ld", Load, 0x1000);
    Test("Re", Recurse, 0x100);
    Test("LR", LoadRecurse, 0x100);
    Test("SA", RecurseSearch, 0x100);
    Test("De", Decompress, 0x100);
}