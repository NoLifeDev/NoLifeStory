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
#include <nx/node.hpp>
#include <nx/file.hpp>
#include <nx/bitmap.hpp>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstddef>
#ifdef _WIN32
#  include <Windows.h>
#else
#  include <ctime>
#endif
namespace {
    char const filename[] = "Data.nx";
    nl::file const file {filename};
    void load() {
        nl::file f {filename};
    }
    size_t c;
    void recurse_sub(nl::node n) {
        ++c;
        for (nl::node nn : n) recurse_sub(nn);
    }
    void recurse_load() {
        recurse_sub(nl::file(filename));
    }
    void recurse() {
        recurse_sub(file);
    }
    void recurse_search_sub(nl::node n) {
        for (nl::node nn : n) n[nn.name_fast()] == nn ? recurse_search_sub(nn) : throw;
    }
    void recurse_search() {
        recurse_search_sub(file);
    }
    void recurse_decompress_sub(nl::node n) {
        n.get_bitmap().data();
        for (nl::node nn : n) recurse_decompress_sub(nn);
    }
    void recurse_decompress() {
        recurse_decompress_sub(file);
    }
#ifdef _WIN32
    double frequency;
    double get_time() {
        LARGE_INTEGER n;
        QueryPerformanceCounter(&n);
        return n.QuadPart * frequency;
    }
    void setup_time() {
        LARGE_INTEGER n;
        QueryPerformanceFrequency(&n);
        frequency = 1000000. / n.QuadPart;
    }
#else
    double get_time() {
        struct timespec t;
        clock_gettime(CLOCK_MONOTONIC_RAW, &t);
        return 1000000. * t.tv_sec + 0.001 * t.tv_nsec;
    }
    void setup_time() {}
#endif
    template <typename T>
    void test(const char * name, T f, size_t maxruns) {
        f();
        std::vector<double> results {};
        do {
            double const c1 {get_time()};
            f();
            double const c2 {get_time()};
            results.emplace_back(c2 - c1);
        } while (--maxruns);
        std::sort(results.begin(), results.end());
        std::vector<double>::const_iterator n0 {results.cbegin() + static_cast<ptrdiff_t>(results.size()) / 4};
        std::vector<double>::const_iterator n1 {results.cbegin() + static_cast<ptrdiff_t>(results.size()) * 3 / 4};
        std::vector<double>::const_iterator n2 {n0 == n1 ? n1 + 1 : n1};
        std::printf("%s\t%u\t%u\t%u\n", name, static_cast<uint32_t>(*n1), static_cast<uint32_t>(std::accumulate(n0, n2, 0.) / (n2 - n0)), static_cast<uint32_t>(results.front()));
    }
}
int main() {
    setup_time();
    std::printf("Name\t75%%t\tM50%%\tBest\n");
    test("Ld", load, 0x1000);
    test("Re", recurse, 0x100);
    test("LR", recurse_load, 0x100);
    test("SA", recurse_search, 0x100);
    test("De", recurse_decompress, 0x10);
}
