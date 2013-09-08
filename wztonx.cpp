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
#include <queue>
#include <map>
#include <cstdint>
#include <unordered_map>

namespace nl {
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
    node * root_node {nullptr};
    std::vector<node> nodes {1};

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
        strings.push_back( {alloc::small(size), size});
        memcpy(strings.back().data, data, size);
        //For debugging purposes
        //std::cout.write(data, size).put('\n');
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

    std::queue<uint32_t> directories;
    std::queue<std::pair<uint32_t, uint32_t>> imgs;
    ptrdiff_t file_start;

    void directory(uint32_t dir_node) {
        node & n {nodes[dir_node]};
        uint16_t count {static_cast<uint16_t>(in::read_cint())};
        n.num = count;
        n.children = static_cast<uint32_t>(nodes.size());
        for (uint16_t i = 0; i < count; ++i) {
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
            uint32_t size {static_cast<uint32_t>(in::read_cint())};
            in::read_cint();//Checksum that nobody cares about
            in::skip(4);
            if (type == 3) directories.push(static_cast<uint32_t>(nodes.size() - 1));
            else if (type == 4) imgs.push(std::pair<uint32_t, uint32_t>(static_cast<uint32_t>(nodes.size() - 1), size));
            else throw std::runtime_error {"Unknown type 2 directory"};
        }
    }
    /*
    void SubProperty(uint32_t node, uint64_t offset);
    void ExtendedProperty(uint32_t node, uint64_t offset) {
        Node & n = Nodes[node];
        uint32_t s = ReadPropString(offset);
        String st = Strings[s];
        if (!strncmp(st.data, "Property", st.size)) {
            InSkip(2);
            SubProperty(node, offset);
        } else if (!strncmp(st.data, "Canvas", st.size)) {
            InSkip(1);
            if (Read<uint8_t>() == 1) {
                InSkip(2);
                SubProperty(node, offset);
            }
            //Canvas stuff
        } else if (!strncmp(st.data, "Shape2D#Vector2D", st.size)) {
            n.type = Type::vector;
            n.vector[0] = ReadCInt();
            n.vector[1] = ReadCInt();
        } else if (!strncmp(st.data, "Shape2D#Convex2D", st.size)) {
            int32_t ec = ReadCInt();
            uint32_t ni = NumNodes;
            NumNodes += ec;
            for (int i = 0; i < ec; ++i, ++ni) {
                Node & nn = Nodes[ni];
                string es = to_string(i);
                nn.name = AddString(es.c_str(), static_cast<uint16_t>(es.size()));
                ExtendedProperty(ni, offset);
            }
        } else if (!strncmp(st.data, "Sound_DX8", st.size)) {
            //Audio stuff
        } else if (!strncmp(st.data, "UOL", st.size)) {
            n.type = Type::uol;
            n.string = ReadPropString(offset);
        } else die("Unknown ExtendedProperty type");
    }
    void SubProperty(uint32_t node, uint64_t offset) {
        int32_t count = ReadCInt();
        uint32_t ni = NumNodes;
        NumNodes += count;
        Node & n = Nodes[node];
        n.num = count;
        n.children = ni;
        for (int i = 0; i < count; ++i, ++ni) {
            Node & nn = Nodes[ni];
            nn.name = ReadPropString(offset);
            uint8_t type = Read<uint8_t>();
            switch (type) {
            case 0x00:
                nn.type = Type::ireal;
                nn.ireal = i;
                break;
            case 0x0B:
            case 0x02:
                nn.type = Type::ireal;
                nn.ireal = Read<uint16_t>();
                break;
            case 0x03:
                nn.type = Type::ireal;
                nn.ireal = ReadCInt();
                break;
            case 0x04:
                nn.type = Type::dreal;
                if (Read<uint8_t>() == 0x80) nn.dreal = Read<float>();
                else nn.dreal = 0.f;
                break;
            case 0x05:
                nn.type = Type::dreal;
                nn.dreal = Read<double>();
                break;
            case 0x08:
                nn.type = Type::string;
                nn.string = ReadPropString(offset);
                break;
            case 0x09:{
                uint64_t p = Read<uint32_t>() + InTell();
                ExtendedProperty(ni, offset);
                InSeek(p);
                break; }
            }
        }
    }
    void Img(uint32_t node, uint32_t size) {
        uint64_t p = InTell();
        InSkip(1);
        DeduceKey();
        InSkip(2);
        SubProperty(node, p);
        InSeek(p + size);
    }*/
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
        directories.emplace(0);
        while (!directories.empty()) {
            directory(directories.front());
            directories.pop();
        }
        while (!imgs.empty()) {
            //Img(Imgs.front().first, Imgs.front().second);
            imgs.pop();
        }
        out::open(filename, 0x100);
        out::close();
        in::close();
    }
}
int main(int argc, char ** argv) {
    clock_t t1 {clock()};
    if (argc > 1) nl::wztonx(argv[1]);
    else nl::wztonx("Data.wz");
    clock_t t2 {clock()};
    std::cout << "Took " << t2 - t1 << " ms" << std::endl;
}
