//////////////////////////////////////////////////////////////////////////////
// NoLifeWzToNx - Part of the NoLifeStory project                           //
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

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define NOMINMAX

#include <Windows.h>

#include <iostream>
#include <fstream>
#include <codecvt>
#include <vector>
#include <deque>
#include <map>
#include <cstdint>
#include <unordered_map>
#include <algorithm>

namespace nl {
    typedef int32_t dirsize_t;
    typedef uint16_t strsize_t;

    //Utility
    template <typename T> struct identity {
        T operator()(T const & v) const {
            return v;
        }
    };

    //File stuff
    namespace in {
        void * file_handle;
        void * map_handle;
        char const * base;
        char const * offset;
        void open(std::string p) {
            file_handle = CreateFileA(p.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
            if (file_handle == INVALID_HANDLE_VALUE) throw std::runtime_error {"Failed to open input file handle"};
            map_handle = CreateFileMappingA(file_handle, nullptr, PAGE_READONLY, 0, 0, nullptr);
            if (map_handle == nullptr) throw std::runtime_error {"Failed to create an input file mapping"};
            base = reinterpret_cast<char *>(MapViewOfFile(map_handle, FILE_MAP_READ, 0, 0, 0));
            if (base == nullptr) throw std::runtime_error {"Failed to map a view of the input file"};
            offset = base;
        }
        void close() {
            UnmapViewOfFile(base);
            CloseHandle(map_handle);
            CloseHandle(file_handle);
        }
        ptrdiff_t tell() {
            return offset - base;
        }
        void seek(ptrdiff_t n) {
            offset = base + n;
        }
        void skip(ptrdiff_t n) {
            offset += n;
        }
        template <typename T> T read() {
            T v {*reinterpret_cast<T const *>(offset)};
            offset += sizeof(T);
            return v;
        }
        int32_t read_cint() {
            int8_t a {read<int8_t>()};
            return a != -128 ? a : read<int32_t>();
        }
    }
    namespace out {
        void * file_handle;
        void * map_handle;
        char * base;
        char * offset;
        void open(std::string & p, uint64_t size) {
            file_handle = CreateFileA(p.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
            if (file_handle == INVALID_HANDLE_VALUE) throw std::runtime_error {"Failed to open output file handle"};
            map_handle = CreateFileMappingA(file_handle, nullptr, PAGE_READWRITE, size >> 32, size & 0xffffffff, nullptr);
            if (map_handle == nullptr) throw std::runtime_error {"Failed to create an output file mapping"};
            base = reinterpret_cast<char *>(MapViewOfFile(map_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0));
            if (base == nullptr) throw std::runtime_error {"Failed to map a view of the output file"};
            offset = base;
        }
        void close() {
            UnmapViewOfFile(base);
            CloseHandle(map_handle);
            CloseHandle(file_handle);
        }
        ptrdiff_t tell() {
            return offset - base;
        }
        void seek(ptrdiff_t n) {
            offset = base + n;
        }
        void skip(ptrdiff_t n) {
            offset += n;
        }
        template <typename T> void write(T const & v) {
            *reinterpret_cast<T *>(offset) = v;
            offset += sizeof(T);
        }
        void write(void * buf, uint64_t size) {
            memcpy(offset, buf, size);
            offset += size;
        }
    }

    //Memory allocation
    namespace alloc {
        char * buffer {nullptr};
        uint64_t remain {0};
        uint64_t const default_size {0x1000000};
        char * big(uint64_t size) {
            return new char[size];
        }
        char * small(uint64_t size) {
            if (size > remain) {
                buffer = big(default_size);
                remain = default_size;
            }
            char * r {buffer};
            buffer += size, remain -= size;
            return r;
        }
    }
    //Node stuff
#pragma pack(push, 1)
    struct node {
        enum class type : uint16_t {
            none = 0,
                integer = 1,
                real = 2,
                string = 3,
                vector = 4,
                bitmap = 5,
                audio = 6,
                uol = 7
        };
        node() : name {0}, children {0}, num {0}, data_type {type::none} {}
        uint32_t name;
        uint32_t children;
        uint16_t num;
        type data_type;
        union {
            int64_t integer;
            double real;
            uint32_t string;
            int32_t vector[2];
            struct bitmap {
                uint32_t id;
                uint16_t width;
                uint16_t height;
            };
            struct audio {
                uint32_t id;
                uint32_t length;
            };
        };
    };
#pragma pack(pop)
    std::vector<node> nodes {1};
    std::vector<std::pair<uint32_t, int32_t>> nodes_to_sort;
    //String stuff
    struct string {
        char * data;
        uint16_t size;
    };
    std::unordered_map<uint32_t, uint32_t, identity<uint64_t>> string_map;
    std::vector<string> strings;
    char16_t wstr_buf[0x8000];
    char str_buf[0x10000];
    std::codecvt_utf8<char16_t> convert;
    extern uint8_t key_bms[65536];
    extern uint8_t key_gms[65536];
    extern uint8_t key_kms[65536];
    uint8_t const (*const keys[3])[65536]{&key_bms, &key_gms, &key_kms};
    uint8_t const (*cur_key)[65536]{nullptr};

    uint32_t add_string(char const * data, uint16_t size) {
        uint32_t hash {2166136261UL};
        char const * s {data};
        for (size_t i {size}; i; --i, ++s) {
            hash ^= static_cast<uint32_t>(*s);
            hash *= 16777619UL;
        }
        auto & id = string_map[hash];
        if (id != 0) return id;
        id = static_cast<uint32_t>(strings.size());
        strings.push_back({alloc::small(size), size});
        memcpy(strings.back().data, data, size);
        //For debugging purposes
        //std::cerr.write(data, size).put('\n');
        return id;
    }
    uint32_t read_enc_string() {
        int32_t len {in::read<int8_t>()};
        if (len == 0) return 0;
        if (len > 0) {
            if (len == 127) len = in::read<int32_t>();
            char16_t const * ows {reinterpret_cast<char16_t const *>(in::offset)};
            in::skip(len * 2);
            char16_t * ws {wstr_buf};
            char16_t mask {0xAAAA};
            char16_t const * key {reinterpret_cast<char16_t const *>(*cur_key)};
            for (int32_t i {len}; i; --i, ++mask, ++ows, ++ws, ++key) {
                *ws = static_cast<char16_t>(*ows ^ *key ^ mask);
            }
            mbstate_t state {};
            const char16_t * fnext {};
            char * tnext {};
            convert.out(state, wstr_buf, wstr_buf + len, fnext, str_buf, str_buf + 0x10000, tnext);
            len = static_cast<int32_t>(tnext - str_buf);
        } else {
            if (len == -128) len = in::read<int32_t>();
            else len = -len;
            char const * os {in::offset};
            in::skip(len);
            char * s {str_buf};
            uint8_t mask {0xAA};
            uint8_t const * key {*cur_key};
            for (int32_t i {len}; i; --i, ++mask, ++os, ++s, ++key) {
                *s = *os ^ *key ^ mask;
            }
        }
        return add_string(str_buf, static_cast<uint16_t>(len));
    }
    uint32_t read_prop_string(ptrdiff_t offset) {
        uint8_t a = in::read<uint8_t>();
        switch (a) {
        case 0x00:
        case 0x73:
            return read_enc_string();
        case 0x01:
        case 0x1B:
            {
                ptrdiff_t o {in::read<uint32_t>() + offset};
                ptrdiff_t p {in::tell()};
                in::seek(o);
                uint32_t s {read_enc_string()};
                in::seek(p);
                return s;
                 }
        default:
            throw std::runtime_error {"Unknown property string type: " + std::to_string(a)};
        }
    }
    void deduce_key() {
        int32_t len {in::read<int8_t>()};
        if (len == -128) len = in::read<int32_t>();
        else len = -len;
        cur_key = nullptr;
        for (auto key : keys) {
            char const * os {in::offset};
            uint8_t mask {0xAA};
            uint8_t const * k {*key};
            bool valid {true};
            for (int32_t i {len}; i; --i && valid, ++mask, ++os, ++k) {
                char c {*os ^ *k ^ mask};
                if (!isalnum(c, std::locale::classic()) && c != '.') valid = false;
            }
            if (valid) cur_key = key;
        }
        if (!cur_key) throw std::runtime_error {"Failed to identify the locale"};
        in::skip(len);
    }

    std::deque<uint32_t> directories;
    std::deque<std::pair<uint32_t, dirsize_t>> imgs;
    ptrdiff_t file_start;

    void sort_nodes(uint32_t first, int32_t count) {
        std::sort(nodes.begin() + first, nodes.begin() + first + count, [](node const & n1, node const & n2) {
            string const & s1 {strings[n1.name]};
            string const & s2 {strings[n2.name]};
            int n {strncmp(s1.data, s2.data, std::min(s1.size, s2.size))};
            if (n < 0) return true;
            if (n > 0) return false;
            if (s1.size < s2.size) return true;
            if (s1.size > s2.size) return false;
            throw std::runtime_error {"Identical strings. This is baaaaaaaaaaaaaad"};
        });
    }
    std::vector<string> resolve_path;
    void resolve_uols(uint32_t uol_node) {
        node & n {nodes[uol_node]};
        if (n.data_type == node::type::uol) {
            std::vector<string> path {resolve_path};
            string & s {strings[n.string]};
            uint16_t b {0};
            for (uint16_t i {0}; i < s.size; ++i) {
                if (s.data[i] == '/') {
                    if (i - b == 2 && strncmp(s.data + b, "..", 2) == 0) path.pop_back();
                    else path.push_back({s.data + b, i - b});
                    b = ++i;
                }
            }
            path.push_back({s.data + b, s.size - b});
            uint32_t search {0};
            for (string & s : path) {
                node & n {nodes[search]};
                bool found {false};
                for (uint32_t i {n.children}; i < n.children + n.num; ++i) {
                    node & nn {nodes[i]};
                    string & ss {strings[nn.name]};
                    if (s.size == ss.size && strncmp(s.data, ss.data, s.size) == 0) {
                        search = i;
                        found = true;
                        break;
                    }
                }
                if (!found) {//Damnit Nexon, get your shit together
                    n.data_type = node::type::none;
                    return;
                }
            }
            node & nn {nodes[search]};
            n.data_type = nn.data_type;
            n.children = nn.children;
            n.num = nn.num;
            n.integer = nn.integer;//Simply copies the whole value. It's a union anyway
            //Note, we do not copy the name
        } else {
            if (uol_node) resolve_path.push_back(strings[n.name]);//Ignore the root node
            for (uint32_t i {0}; i < n.num; ++i) resolve_uols(n.children + i);
            if (uol_node) resolve_path.pop_back();
        }
    }
    void directory(uint32_t dir_node) {
        node & n {nodes[dir_node]};
        int32_t count {in::read_cint()};
        uint32_t ni {static_cast<uint32_t>(nodes.size())};
        n.num = count;
        n.children = ni;
        for (uint32_t i = 0; i < count; ++i) {
            nodes.emplace_back();
            node & nn {nodes.back()};
            uint8_t type {in::read<uint8_t>()};
            switch (type) {
            case 1:
                throw std::runtime_error {"Found the elusive type 1 directory"};
            case 2:
                {
                    int32_t s {in::read<int32_t>()};
                    ptrdiff_t p {in::tell()};
                    in::seek(file_start + s);
                    type = in::read<uint8_t>();
                    nn.name = read_enc_string();
                    in::seek(p);
                    break;
                  }
            case 3:
            case 4:
                nn.name = read_enc_string();
                break;
            default:
                throw std::runtime_error {"Unknown directory type"};
            }
            dirsize_t size {in::read_cint()};
            if (size <= 0) throw std::runtime_error {"Directory/img has invalid size!"};
            in::read_cint();//Checksum that nobody cares about
            in::skip(4);
            if (type == 3) directories.push_back(ni + i);
            else if (type == 4) imgs.emplace_back(ni + i, size);
            else throw std::runtime_error {"Unknown type 2 directory"};
        }
        nodes_to_sort.emplace_back(ni, count);
    }
    void sub_property(uint32_t, ptrdiff_t);
    void extended_property(uint32_t prop_node, uint64_t offset) {
        node & n {nodes[prop_node]};
        uint32_t s {read_prop_string(offset)};
        string const & st {strings[s]};
        if (!strncmp(st.data, "Property", st.size)) {
            in::skip(2);
            sub_property(prop_node, offset);
        } else if (!strncmp(st.data, "Canvas", st.size)) {
            in::skip(1);
            if (in::read<uint8_t>() == 1) {
                in::skip(2);
                sub_property(prop_node, offset);
            }
            //Canvas stuff todo later
        } else if (!strncmp(st.data, "Shape2D#Vector2D", st.size)) {
            n.data_type = node::type::vector;
            n.vector[0] = in::read_cint();
            n.vector[1] = in::read_cint();
        } else if (!strncmp(st.data, "Shape2D#Convex2D", st.size)) {
            int32_t count {in::read_cint()};
            uint32_t ni {static_cast<uint32_t>(nodes.size())};
            nodes.resize(nodes.size() + count);
            for (uint32_t i = 0; i < count; ++i) {
                node & nn {nodes[ni + i]};
                std::string es {std::to_string(i)};
                nn.name = add_string(es.c_str(), static_cast<uint16_t>(es.size()));
                extended_property(ni, offset);
            }
            nodes_to_sort.emplace_back(ni, count);
        } else if (!strncmp(st.data, "Sound_DX8", st.size)) {
            //Audio stuff
        } else if (!strncmp(st.data, "UOL", st.size)) {
            in::skip(1);
            n.data_type = node::type::uol;
            n.string = read_prop_string(offset);
        } else throw std::runtime_error {"Unknown sub property type: " + std::string {st.data, st.size}};
    }
    void sub_property(uint32_t prop_node, ptrdiff_t offset) {
        int32_t count {in::read_cint()};
        node & n {nodes[prop_node]};
        uint32_t ni {static_cast<uint32_t>(nodes.size())};
        n.num = count;
        n.children = ni;
        nodes.resize(nodes.size() + count);
        for (uint32_t i = 0; i < count; ++i) {
            node & nn {nodes[ni + i]};
            nn.name = read_prop_string(offset);
            uint8_t type {in::read<uint8_t>()};
            switch (type) {
            case 0x00://Turning null nodes into integers with an id. Useful for zmap.img
                nn.data_type = node::type::integer;
                nn.integer = i;
                break;
            case 0x0B:
            case 0x02:
                nn.data_type = node::type::integer;
                nn.integer = in::read<uint16_t>();
                break;
            case 0x03:
                nn.data_type = node::type::integer;
                nn.integer = in::read_cint();
                break;
            case 0x04:
                nn.data_type = node::type::real;
                nn.real = in::read<uint8_t>() == 0x80 ? in::read<float>() : 0.;
                break;
            case 0x05:
                nn.data_type = node::type::real;
                nn.real = in::read<double>();
                break;
            case 0x08:
                nn.data_type = node::type::string;
                nn.string = read_prop_string(offset);
                break;
            case 0x09:{
                ptrdiff_t p {in::read<uint32_t>() + in::tell()};
                extended_property(ni + i, offset);
                in::seek(p);
                break;
                      }
            default:
                throw std::runtime_error {"Unknown sub property type: " + std::to_string(type)};
            }
        }
        nodes_to_sort.emplace_back(ni, count);
    }
    void img(uint32_t img_node, dirsize_t size) {
        ptrdiff_t p {in::tell()};
        in::skip(1);
        deduce_key();
        in::skip(2);
        sub_property(img_node, p);
        in::seek(p + size);
    }
    void wztonx(std::string filename) {
        in::open(filename);
        filename.erase(filename.find_last_of('.')).append(".nx");
        uint32_t magic {in::read<uint32_t>()};
        if (magic != *reinterpret_cast<uint32_t *>("PKG1")) throw std::runtime_error {"Not a valid WZ file"};
        in::skip(8);
        file_start = in::read<uint32_t>();
        in::seek(file_start + 2);
        in::read_cint();
        in::skip(1);
        deduce_key();
        in::seek(file_start + 2);
        add_string("", 0);
        std::cout << "Opened file" << std::endl;
        directories.push_back(0);
        while (!directories.empty()) {
            directory(directories.front());
            directories.pop_front();
        }
        std::cout << "Parsed directories" << std::endl;
        while (!imgs.empty()) {
            img(imgs.front().first, imgs.front().second);
            imgs.pop_front();
        }
        std::cout << "Parsed images" << std::endl;
        resolve_uols(0);
        for (auto const & n : nodes_to_sort) sort_nodes(n.first, n.second);
        std::cout << "Node cleanup finished" << std::endl;
        ptrdiff_t node_offset = 52;
        node_offset += 0x10 - (node_offset & 0xf);
        ptrdiff_t string_table_offset = node_offset + nodes.size() * 20;
        string_table_offset += 0x10 - (string_table_offset & 0xf);
        ptrdiff_t string_offset = string_table_offset + strings.size() * 8;
        string_offset += 0x10 - (string_offset & 0xf);
        ptrdiff_t bitmap_table_offset = string_offset + strings.size() * 2;
        for (auto const & s : strings) bitmap_table_offset += s.size;
        bitmap_table_offset += 0x10 - (bitmap_table_offset & 0xf);
        out::open(filename, bitmap_table_offset);
        out::seek(0);
        out::write<uint32_t>(0x34474B50);
        out::write<uint32_t>(nodes.size());
        out::write<uint64_t>(node_offset);
        out::write<uint32_t>(strings.size());
        out::write<uint64_t>(string_table_offset);
        std::cout << "Opened output" << std::endl;
        out::seek(node_offset);
        out::write(nodes.data(), nodes.size() * 20);
        std::cout << "Wrote nodes" << std::endl;
        out::seek(string_table_offset);
        ptrdiff_t next_str {string_offset};
        for (auto const & s : strings) {
            out::write<uint64_t>(next_str);
            next_str += s.size + 2;
        }
        out::seek(string_offset);
        for (auto const & s : strings) {
            out::write<uint16_t>(s.size);
            out::write(s.data, s.size);
        }
        std::cout << "Wrote strings" << std::endl;
        out::close();
        in::close();
        std::cout << "Done" << std::endl;
    }
}
int main(int argc, char ** argv) {
    clock_t t1 {clock()};
    if (argc > 1) nl::wztonx(argv[1]);
    else nl::wztonx("Data.wz");
    clock_t t2 {clock()};
    std::cout << "Took " << t2 - t1 << " ms" << std::endl;
}
