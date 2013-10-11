//////////////////////////////////////////////////////////////////////////////
// NoLifeNxBench - Part of the NoLifeStory project                          //
// Copyright © 2013 Peter Atashian                                          //
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
#include <functional>
#ifdef _WIN32
#  include <Windows.h>
#else
#  include <ctime>
#endif

namespace nl {
    namespace {
        std::string const filename {"Data.nx"};
        file const nxfile {filename};
        void load() {
            file f {filename};
        }
        void recurse_sub(node n) {
            for (node nn : n) recurse_sub(nn);
        }
        void recurse_load() {
            recurse_sub(file(filename));
        }
        void recurse() {
            recurse_sub(nxfile);
        }
        void recurse_search_sub(node n) {
            for (node nn : n) n[nn.name_fast()] == nn ? recurse_search_sub(nn) : throw;
        }
        void recurse_search() {
            recurse_search_sub(nxfile);
        }
        void recurse_decompress_sub(node n) {
            n.get_bitmap().data();
            for (node nn : n) recurse_decompress_sub(nn);
        }
        void recurse_decompress() {
            recurse_decompress_sub(nxfile);
        }
#ifdef _WIN32
        double frequency;
        double get_time() {
            LARGE_INTEGER n {};
            QueryPerformanceCounter(&n);
            return n.QuadPart * frequency;
        }
        void setup_time() {
            LARGE_INTEGER n {};
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
        void test(std::string name, std::function<void()> func, size_t maxruns) {
            std::vector<double> results {};
            do {
                double const c1 {get_time()};
                func();
                double const c2 {get_time()};
                results.emplace_back(c2 - c1);
            } while (--maxruns);
            std::sort(results.begin(), results.end());
            std::vector<double>::const_iterator const q0 {results.cbegin()};
            std::vector<double>::const_iterator const q4 {results.cend()};
            std::vector<double>::const_iterator const q2 {q0 + (q4 - q0) / 2};
            std::vector<double>::const_iterator const q1 {q0 + (q2 - q0) / 2};
            std::vector<double>::const_iterator const q3 {q2 + (q4 - q2) / 2};
            std::printf("%s\t%u\t%u\t%u\n", name.c_str(),
                        static_cast<unsigned>(*q3),
                        static_cast<unsigned>(std::accumulate(q1, q3, 0.) / (q3 - q1)),
                        static_cast<unsigned>(*q0));
        }
        void bench() {
            setup_time();
            std::printf("Name\t75%%t\tM50%%\tBest\n");
            test("Ld", load, 0x1000);
            test("Re", recurse, 0x100);
            test("LR", recurse_load, 0x100);
            test("SA", recurse_search, 0x100);
            test("De", recurse_decompress, 0x10);
        }
    }
}
int main() {
    nl::bench();
}
