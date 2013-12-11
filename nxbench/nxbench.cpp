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
#include <nx/audio.hpp>
#include <nx/nx.hpp>
#ifdef _WIN32
#  include <Windows.h>
#else
#  include <ctime>
#endif
#include <cstdio>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstddef>
#include <functional>
#include <fstream>
#include <iostream>
#include <map>
#include <set>

namespace nl {
    class bench {
        std::string const filename = "Data.nx";
        file const nxfile = {filename};
        size_t load() {
            return file(filename).node_count();
        }
        size_t recurse_sub(node const & n) {
            size_t c = 1;
            for (node const & nn : n)
                c += nn.size() ? recurse_sub(nn) : 1;
            return c;
        }
        size_t recurse_load() {
            return recurse_sub(file(filename));
        }
        size_t recurse() {
            return recurse_sub(nxfile);
        }
        size_t recurse_search_sub(node const & n) {
            if (n["x"])
                return 1;
            size_t c = 0;
            for (node const & nn : n)
                c += nn.size() ? recurse_search_sub(nn) : 0;
            return c;
        }
        size_t recurse_search() {
            return recurse_search_sub(nxfile);
        }
        void recurse_decompress_sub(node const & n) {
            n.get_bitmap().data();
            for (node const & nn : n)
                recurse_decompress_sub(nn);
        }
        size_t recurse_decompress() {
            recurse_decompress_sub(nxfile);
            return 0;
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
        void test(std::string name, T func, size_t maxruns) {
            std::vector<double> results;
            size_t answer;
            do {
                double const c1 = get_time();
                answer = (this->*func)();
                double const c2 = get_time();
                results.emplace_back(c2 - c1);
            } while (--maxruns);
            std::sort(results.begin(), results.end());
            auto const q0 = results.cbegin();
            auto const q4 = results.cend();
            auto const q2 = q0 + (q4 - q0) / 2;
            auto const q1 = q0 + (q2 - q0) / 2;
            auto const q3 = q2 + (q4 - q2) / 2;
            std::printf("%s\t%u\t%u\t%u\t%u\n", name.c_str(), static_cast<unsigned>(*q3), static_cast<unsigned>(std::accumulate(q1, q3, 0.) / (q3 - q1)), static_cast<unsigned>(*q0), static_cast<unsigned>(answer));
        }
    public:
        bench() {
            setup_time();
            std::printf("Name\t75%%t\tM50%%\tBest\tAnswer\n");
            test("Ld", &bench::load, 0x1000);
            test("Re", &bench::recurse, 0x40);
            test("LR", &bench::recurse_load, 0x40);
            test("SA", &bench::recurse_search, 0x40);
            test("De", &bench::recurse_decompress, 0x10);
        }
    };
    void dump_music() {
        file soundfile("Sound.nx");
        node sounds = soundfile;
        for (auto n : sounds)
        if (n.name().find("Bgm") != std::string::npos)
        for (auto nn : n)
        if (nn.data_type() == node::type::audio)
            std::ofstream(n.name() + "." + nn.name() + ".mp3", std::ios::binary).write(reinterpret_cast<char const *>(nn.get_audio().data()) + 82, nn.get_audio().length() - 82);
    }
    std::set<std::tuple<std::string, std::string>> stuff;
    std::ofstream dump("NoLifeNxBench.log");
    void gather_node_dump_recursive(node n, std::string s) {
        stuff.emplace(s, n);
        s.push_back('/');
        for (node nn : n)
            gather_node_dump_recursive(nn, s + nn.name());
    }
    void dump_node_recursive(node n) {
        for (node nn : n) {
            gather_node_dump_recursive(nn, nn.name());
        }
    }
    void gather_node_dump(node n, std::string s) {
        stuff.emplace(s, n);
    }
    void dump_node(node n) {
        for (node nn : n) {
            gather_node_dump(nn, nn.name());
        }
    }
    void dump_stuff() {
        nx::load_all();
        dump_node(nx::character);
        for (auto & s : stuff)
            dump << std::get<0>(s) << ": " << std::get<1>(s) << std::endl;
    }
}
int main() {
    nl::dump_stuff();
    //nl::bench();
}
