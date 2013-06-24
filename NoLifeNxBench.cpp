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
#include <iostream>
#include <iomanip>
#include <limits>
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
void Test(const char * name, T f) {
    std::vector<int64_t> results;
    int64_t c0 = GetHPC();
    do {
        int64_t c1 = GetHPC();
        f();
        int64_t c2 = GetHPC();
        results.emplace_back(c2 - c1);
    } while (GetHPC() - c0 < Freq << 3);
    std::sort(results.begin(), results.end());
    printf("{%s, %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld}\n", name, static_cast<int64_t>(results.size()),
        Adjust(std::accumulate(results.cbegin(), results.cend(), 0) / static_cast<int64_t>(results.size())),
        Adjust(std::accumulate(results.cbegin() + static_cast<ptrdiff_t>(results.size()) / 4, results.cend() - static_cast<ptrdiff_t>(results.size()) / 4, 0) / static_cast<int64_t>(results.size() - results.size() / 4 * 2)),
        Adjust(results[(results.size() - 1) * 0 / 4]),
        Adjust(results[(results.size() - 1) * 1 / 4]),
        Adjust(results[(results.size() - 1) * 2 / 4]),
        Adjust(results[(results.size() - 1) * 3 / 4]),
        Adjust(results[(results.size() - 1) * 4 / 4]));
}
int main() {
    GetFreq();
    printf("{Name, Count, Mean, 50%%Mean, 0%%, 25%%, 50%%, 75%%, 100%%}\n");
    Test("Load", Load);
    Test("LoadRecurse", LoadRecurse);
    Test("Recurse", Recurse);
    Test("RecurseSearch", RecurseSearch);
    Test("Decompress", Decompress);
}