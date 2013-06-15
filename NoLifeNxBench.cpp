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
#include <Windows.h>
NL::File const file("Data.nx");
NL::Node const node = file.Base()["Map"]["Map"]["Map1"]["105060000.img"]["1"]["tile"];
size_t c;
uint64_t freq;
void stringsearch() {
    for (NL::Node n : node) if (node[n.NameFast()] != n) throw;
}
void fileload() {
    delete new NL::File("Data.nx");
}
void recursealternate2(NL::Node n) {
    NL::Node nn = n.begin();
    for (auto i =  n.Size(); i; --i, ++nn) recursealternate2(nn);
}
void recursealternate() {
    recursealternate2(file.Base());
}
void recurse(NL::Node n) {
    c++;
    for (NL::Node nn : n) recurse(nn);
}
void initialrecurse() {
    NL::File f("Data.nx");
    recurse(f.Base());
}
void morerecurse() {
    recurse(file.Base());
}
extern "C" {
    extern void asmrecurse(char const *,  char const *);
}
void recurseoptimal() {
    NL::Node n = file.Base();
    char const * f = *(reinterpret_cast<char const **>(&n) + 1);
    char const * d = *reinterpret_cast<char const **>(&n);
    asmrecurse(f, d);
}
uint64_t gethpc() {
    LARGE_INTEGER n;
    QueryPerformanceCounter(&n);
    return n.QuadPart;
}
void getfreq() {
    LARGE_INTEGER n;
    QueryPerformanceFrequency(&n);
    freq = n.QuadPart;
}
template <typename T>
void test(const char * name, T f) {
    uint64_t best = -1;
    uint64_t c0 = gethpc();
    do {
        uint64_t c1 = gethpc();
        f();
        uint64_t c2 = gethpc();
        uint64_t dif = c2 - c1;
        if (dif < best) best = dif;
    } while (gethpc() - c0 < freq);
    printf("%s: %uus\n", name, best * 1000000ULL / freq);
}
std::pair<uint64_t, uint64_t> results[0x10000] = {};
void stringrecurse(NL::Node n) {
    for (NL::Node nn : n) stringrecurse(nn);
    uint64_t c0 = gethpc();
    for (size_t i = 0x10; i; --i) for (NL::Node nn : n) if (n[nn.NameFast()] != nn) throw;
    uint64_t c1 = gethpc();
    results[n.Size()].first += c1 - c0;
    results[n.Size()].second += 1;
}
void stringtest() {
    stringrecurse(file.Base());
    for (size_t i = 1; i < 0x10000; ++i) {
        auto && r = results[i];
        double t = r.first * 1000000000. / (r.second * freq * i * 0x10);
        if (r.second) printf("%u: %fns\n", i, t);
    }
}
int main() {
    getfreq();
    test("File Loading", fileload);
    test("String Searching", stringsearch);
    test("Initial Recursion", initialrecurse);
    test("C++ Recursion", morerecurse);
    test("Alternate C++ Recursion", recursealternate);
    test("ASM Recursion", recurseoptimal);
    stringtest();
}