//////////////////////////////////////////////////////////////////////////////
// NoLifeNxBench - Part of the NoLifeStory project                          //
// Copyright © 2014 Peter Atashian                                          //
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
#define NOMINMAX
#include <Windows.h>
#else
#include <ctime>
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
#include <iomanip>
#include <set>
#include <chrono>
#include <regex>

namespace nl {
class bench {
    std::string const filename = "Data.nx";
    file const nxfile = {filename};
    size_t load() { return file(filename).node_count(); }
    size_t recurse_sub(node const & n) {
        size_t c = 1;
        for (node const & nn : n) { c += recurse_sub(nn); }
        return c;
    }
    size_t recurse_load() { return recurse_sub(file(filename)); }
    size_t recurse() { return recurse_sub(nxfile); }
    size_t recurse_search_sub(node const & n) {
        size_t c = 1;
        for (node const & nn : n)
            c += n[nn.name()] == nn ? nn.size() ? recurse_search_sub(nn) : 1 : 0;
        return c;
    }
    size_t recurse_search() { return recurse_search_sub(nxfile); }
    size_t recurse_decompress_sub(node const & n) {
        size_t c = n.get_bitmap().data() ? 1u : 0u;
        for (node const & nn : n) c += recurse_decompress_sub(nn);
        return c;
    }
    size_t recurse_decompress() { return recurse_decompress_sub(nxfile); }
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
        std::printf("%s\t%u\t%u\t%u\t%u\n", name.c_str(), static_cast<unsigned>(*q3),
                    static_cast<unsigned>(std::accumulate(q1, q3, 0.) / (q3 - q1)),
                    static_cast<unsigned>(*q0), static_cast<unsigned>(answer));
    }

public:
    bench() {
        setup_time();
        std::printf("Name\t75%%t\tM50%%\tBest\tChecksum\n");
        test("Ld", &bench::load, 0x1000);
        test("Re", &bench::recurse, 0x20);
        test("LR", &bench::recurse_load, 0x20);
        test("SA", &bench::recurse_search, 0x20);
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
                    std::ofstream(n.name() + "." + nn.name() + ".mp3", std::ios::binary)
                        .write(reinterpret_cast<char const *>(nn.get_audio().data()) + 82,
                               nn.get_audio().length() - 82);
}
struct dump {
    struct info {
        node n;
        std::string path;
        info(node n, std::string path) : n(n), path(path) {}
    };
    std::vector<info> nodes;
    dump(std::string s) {
        nx::load_all();
        root(s);
    }
    ~dump() { write(); }
    dump & root(std::string s) {
        nodes.clear();
        if (s == "Base")
            nodes.emplace_back(nx::base, "Base");
        else if (s == "Character")
            nodes.emplace_back(nx::character, "Base/Character");
        else if (s == "Effect")
            nodes.emplace_back(nx::effect, "Base/Effect");
        else if (s == "Etc")
            nodes.emplace_back(nx::etc, "Base/Etc");
        else if (s == "Item")
            nodes.emplace_back(nx::item, "Base/Item");
        else if (s == "Map")
            nodes.emplace_back(nx::map, "Base/Map");
        else if (s == "Mob")
            nodes.emplace_back(nx::mob, "Base/Mob");
        else if (s == "Morph")
            nodes.emplace_back(nx::morph, "Base/Morph");
        else if (s == "Npc")
            nodes.emplace_back(nx::npc, "Base/Npc");
        else if (s == "Quest")
            nodes.emplace_back(nx::quest, "Base/Quest");
        else if (s == "Reactor")
            nodes.emplace_back(nx::reactor, "Base/Reactor");
        else if (s == "Skill")
            nodes.emplace_back(nx::skill, "Base/Skill");
        else if (s == "Sound")
            nodes.emplace_back(nx::sound, "Base/Sound");
        else if (s == "String")
            nodes.emplace_back(nx::string, "Base/String");
        else if (s == "TamingMob")
            nodes.emplace_back(nx::tamingmob, "Base/TamingMob");
        else if (s == "UI")
            nodes.emplace_back(nx::ui, "Base/UI");
        return *this;
    }
    dump & name(std::string s) {
        std::vector<info> new_nodes;
        for (auto it : nodes) {
            auto n = it.n[s];
            if (n) new_nodes.emplace_back(n, it.path + '/' + n.name());
        }
        nodes.swap(new_nodes);
        return *this;
    }
    dump & all() {
        std::vector<info> new_nodes;
        for (auto it : nodes)
            for (auto n : it.n) new_nodes.emplace_back(n, it.path + '/' + n.name());
        nodes.swap(new_nodes);
        return *this;
    }
    dump & regex(std::string s) {
        std::vector<info> new_nodes;
        std::regex reg(s, std::regex_constants::optimize | std::regex_constants::extended);
        for (auto it : nodes)
            for (auto n : it.n)
                if (std::regex_match(n.name(), reg))
                    new_nodes.emplace_back(n, it.path + '/' + n.name());
        nodes.swap(new_nodes);
        return *this;
    }
    dump & filter_type(node::type t) {
        auto it = std::remove_if(nodes.begin(), nodes.end(),
                                 [&](info & i) { return t != i.n.data_type(); });
        nodes.erase(it, nodes.end());
        return *this;
    }
    dump & filter_value(std::string v) {
        auto it = std::remove_if(nodes.begin(), nodes.end(),
                                 [&](info & i) { return v != i.n.get_string(); });
        nodes.erase(it, nodes.end());
        return *this;
    }
    dump & has_child(std::string s) {
        auto it = std::remove_if(nodes.begin(), nodes.end(), [&](info & i) { return !i.n[s]; });
        nodes.erase(it, nodes.end());
        return *this;
    }
    dump & not_has_child(std::string s) {
        auto it = std::remove_if(nodes.begin(), nodes.end(), [&](info & i) { return i.n[s]; });
        nodes.erase(it, nodes.end());
        return *this;
    }
    std::string get_value(node n) {
        auto s = n.name() + '.';
        switch (n.data_type()) {
        case node::type::audio: return s + "audio";
        case node::type::bitmap: return s + "bitmap";
        case node::type::integer: return s + "integer=" + n.get_string();
        case node::type::none: return s + "none";
        case node::type::real: return s + "real=" + n.get_string();
        case node::type::string: return s + "string=" + n.get_string();
        case node::type::vector:
            return s + "vector=" + std::to_string(n.x()) + "," + std::to_string(n.y());
        default: throw std::runtime_error("Wat");
        }
    }
    std::string make_line(std::string s) {
        auto a = 20 - s.size() / 2;
        auto b = 40 - s.size() - a;
        return std::string(a, '=') + s + std::string(b, '=');
    }
    void write() {
        std::ofstream file("NoLifeNxBench.log");
        std::map<std::string, size_t> values;
        std::set<std::string> children;
        std::set<std::string> paths;
        for (auto it : nodes) {
            ++values[get_value(it.n)];
            paths.emplace(it.path);
            for (auto n : it.n) children.emplace(n.name());
        }
        file << make_line("Values") << std::endl;
        for (auto & s : values)
            file << std::setw(4) << std::right << s.second << "x " << s.first << std::endl;
        file << make_line("Children") << std::endl;
        for (auto & s : children) file << "* [[/" << s << "|" << s << "]]" << std::endl;
        file << make_line("Paths") << std::endl;
        for (auto & s : paths) file << "* " << s << std::endl;
    }
};
void dump_tree(node n, std::string path) {
    std::cout << path << '\n';
    path += '/';
    for (auto nn : n) dump_tree(nn, path + nn.name());
}
void diff_node(node a, node b, std::string path) {
    path += '/';
    for (auto an : a) {
        auto name = an.name();
        auto bn = b[name];
        if (bn)
            diff_node(an, bn, path + name);
        else
            dump_tree(an, '-' + path + name);
    }
    for (auto bn : b) {
        auto name = bn.name();
        auto an = a[name];
        if (!an) dump_tree(bn, '+' + path + name);
    }
}
void diff(file a, file b) { diff_node(a, b, {}); }
}

void fraysa();

int main() {
    auto a = std::chrono::high_resolution_clock::now();
    nl::bench{};
    auto b = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(b - a).count()
              << std::endl;
    std::system("pause");
    return EXIT_SUCCESS;
}
