//////////////////////////////////////////////////////////////////////////////
// NoLifeWzToNx - Part of the NoLifeStory project                           //
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

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  define VC_EXTRALEAN
#  define NOMINMAX
#  include <Windows.h>
#else
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <sys/fcntl.h>
#  include <sys/mman.h>
#  include <unistd.h>
#endif

#include <iostream>
#include <fstream>
#include <codecvt>
#include <vector>
#include <map>
#include <cstdint>
#include <unordered_map>
#include <algorithm>
#include <chrono>

namespace nl {
    //Some typedefs
    typedef uint16_t strsize_t;
    typedef char char8_t;
    typedef uint32_t id_t;
    typedef uint32_t hash_t;
    typedef uint8_t key_t;
    //The keys
    //TODO - Use AES to generate these keys at runtime
    extern key_t key_bms[65536];
    extern key_t key_gms[65536];
    extern key_t key_kms[65536];
    key_t const * keys[3] = {key_bms, key_gms, key_kms};
    //Identity operation because C++ doesn't have such a template. Surprising, I know.
    template <typename T> struct identity {
        T operator()(T const & v) const {
            return v;
        }
    };
    //Memory allocation
    namespace alloc {
        char * buffer {nullptr};
        size_t remain {0};
        size_t const default_size {0x1000000};
        char * big(size_t size) {
            return new char[size];
        }
        void * small(size_t size) {
            if (size > remain) {
                buffer = big(default_size);
                remain = default_size;
            }
            char * r {buffer};
            buffer += size, remain -= size;
            return r;
        }
    }
    //Input memory mapped file
    struct imapfile {
        char const * base = nullptr;
        char const * offset = nullptr;
#ifdef _WIN32
        void * file_handle = nullptr;
        void * map_handle = nullptr;
        void open(std::string p) {
            file_handle = CreateFileA(p.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
            if (file_handle == INVALID_HANDLE_VALUE) throw std::runtime_error("Failed to open file " + p);
            map_handle = CreateFileMappingA(file_handle, nullptr, PAGE_READONLY, 0, 0, nullptr);
            if (map_handle == nullptr) throw std::runtime_error("Failed to create file mapping of file " + p);
            base = reinterpret_cast<char *>(MapViewOfFile(map_handle, FILE_MAP_READ, 0, 0, 0));
            if (base == nullptr) throw std::runtime_error("Failed to map view of file " + p);
            offset = base;
        }
        ~imapfile() {
            UnmapViewOfFile(base);
            CloseHandle(map_handle);
            CloseHandle(file_handle);
        }
#else
        int file_handle = 0;
        size_t file_size = 0;
        void open(std::string p) {
            file_handle = ::open(p.c_str(), O_RDONLY);
            if (file_handle == -1) throw std::runtime_error("Failed to open file " + p);
            struct stat finfo;
            if (fstat(file_handle, &finfo) == -1) throw std::runtime_error("Failed to obtain file information of file " + p);
            file_size = finfo.st_size;
            base = reinterpret_cast<char const *>(mmap(nullptr, file_size, PROT_READ, MAP_SHARED, file_handle, 0));
            if (reinterpret_cast<intptr_t>(base) == -1) throw std::runtime_error("Failed to create memory mapping of file " + p);
            offset = base;
        }
        ~imapfile() {
            munmap(const_cast<char *>(base), file_size);
            close(file_handle);
        }
#endif
        size_t tell() {
            return static_cast<size_t>(offset - base);
        }
        void seek(size_t n) {
            offset = base + n;
        }
        void skip(size_t n) {
            offset += n;
        }
        template <typename T> T read() {
            auto & v = *reinterpret_cast<T const *>(offset);
            offset += sizeof(T);
            return v;
        }
        int32_t read_cint() {
            int8_t a = read<int8_t>();
            return a != -128 ? a : read<int32_t>();
        }
    };
    //Output memory mapped file
    struct omapfile {
        char * base = nullptr;
        char * offset = nullptr;
#ifdef _WIN32
        void * file_handle = nullptr;
        void * map_handle = nullptr;
        void open(std::string p, size_t size) {
            file_handle = CreateFileA(p.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
            if (file_handle == INVALID_HANDLE_VALUE) throw std::runtime_error("Failed to open file " + p);
            map_handle = CreateFileMappingA(file_handle, nullptr, PAGE_READWRITE, size >> 32, size & 0xffffffff, nullptr);
            if (map_handle == nullptr) throw std::runtime_error("Failed to create file mapping of file " + p);
            base = reinterpret_cast<char *>(MapViewOfFile(map_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0));
            if (base == nullptr) throw std::runtime_error("Failed to map view of file " + p);
            offset = base;
        }
        ~omapfile() {
            UnmapViewOfFile(base);
            CloseHandle(map_handle);
            CloseHandle(file_handle);
        }
#else
        int file_handle = 0;
        size_t file_size = 0;
        void open(std::string p, uint64_t size) {
            file_handle = ::open(p.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (file_handle == -1) throw std::runtime_error("Failed to open file " + p);
            file_size = size;
            if (lseek(file_handle, file_size - 1, SEEK_SET) == -1) throw std::runtime_error("Error calling lseek() to 'stretch' file " + p);
            if (write(file_handle, "", 1) != 1) throw std::runtime_error("Error writing last byte of file " + p);
            base = reinterpret_cast<char *>(mmap(nullptr, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_handle, 0));
            if (reinterpret_cast<intptr_t>(base) == -1) throw std::runtime_error("Failed to create memory mapping of file " + p);
            offset = base;
        }
        void close() {
            munmap(const_cast<char *>(base), file_size);
            ::close(file_handle);
        }
#endif
        size_t tell() {
            return static_cast<size_t>(offset - base);
        }
        void seek(size_t n) {
            offset = base + n;
        }
        void skip(size_t n) {
            offset += n;
        }
        template <typename T> void write(T const & v) {
            *reinterpret_cast<T *>(offset) = v;
            offset += sizeof(T);
        }
        void write(void * buf, size_t size) {
            memcpy(offset, buf, size);
            offset += size;
        }
    };
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
        uint32_t name = 0;
        uint32_t children = 0;
        uint16_t num = 0;
        type data_type = type::none;
        union _data {
            int64_t integer = 0;
            double real;
            uint32_t string;
            int32_t vector[2];
            struct _bitmap {
                uint32_t id;
                uint16_t width;
                uint16_t height;
            } bitmap;
            struct _audio {
                uint32_t id;
                uint32_t length;
            } audio;
        } data;
    };
#pragma pack(pop)
    //The string class
    struct string {
        char8_t * data;
        strsize_t size;
    };
    char16_t wstr_buf[0x8000] = {};
    char8_t str_buf[0x10000] = {};
    //The main class itself
    struct wztonx {
        //Variables
        imapfile in;
        omapfile out;
        std::vector<node> nodes = {{node()}};
        std::vector<std::pair<id_t, id_t>> nodes_to_sort;
        std::unordered_map<hash_t, id_t, identity<hash_t>> string_map;
        std::vector<string> strings;
        std::codecvt_utf8<char16_t> convert;
        char8_t const * u8key = nullptr;
        char16_t const * u16key = nullptr;
        std::vector<std::pair<id_t, int32_t>> imgs;
        size_t file_start = 0;
        std::vector<id_t> uol_path;
        std::vector<std::vector<id_t>> uols;
        std::vector<uint64_t> bitmaps;
        std::vector<uint64_t> sounds;
        //Methods
        id_t add_string(char8_t const * data, strsize_t size) {
            hash_t hash = 2166136261u;
            for (auto i = 0u; i < size; ++i) {
                hash ^= static_cast<hash_t>(data[i]);
                hash *= 16777619u;
            }
            auto & id = string_map[hash];
            if (id != 0)
                return id;
            id = static_cast<id_t>(strings.size());
            //TODO - Use vectors instead of allocations
            strings.push_back({static_cast<char8_t *>(alloc::small(size)), size});
            memcpy(strings.back().data, data, size);
            return id;
        }
        id_t read_enc_string() {
            auto len = in.read<int8_t>();
            if (len > 0) {
                auto slen = len == 127 ? in.read<uint32_t>() : len;
                if (slen * 2 > std::numeric_limits<strsize_t>::max())
                    throw std::runtime_error("String is too long!");
                auto ows = reinterpret_cast<char16_t const *>(in.offset);
                in.skip(slen * 2u);
                uint16_t mask = 0xAAAA;
                for (auto i = 0u; i < slen; ++i, ++mask)
                    wstr_buf[i] = static_cast<char16_t>(ows[i] ^ u16key[i] ^ mask);
                mbstate_t state;
                char16_t const * fnext;
                char8_t * tnext;
                convert.out(state, wstr_buf, wstr_buf + slen, fnext, str_buf, str_buf + 0x10000, tnext);
                return add_string(str_buf, static_cast<strsize_t>(tnext - str_buf));
            }
            if (len < 0) {
                auto slen = len == -128 ? in.read<uint32_t>() : -len;
                if (slen > std::numeric_limits<strsize_t>::max())
                    throw std::runtime_error("String is too long!");
                auto os = reinterpret_cast<char8_t const *>(in.offset);
                in.skip(slen);
                uint8_t mask = 0xAA;
                for (auto i = 0u; i < slen; ++i, ++mask)
                    str_buf[i] = os[i] ^ u8key[i] ^ mask;
                return add_string(str_buf, static_cast<strsize_t>(slen));
            }
            return 0;
        }
        id_t read_prop_string(size_t offset) {
            auto a = in.read<uint8_t>();
            switch (a) {
            case 0x00:
            case 0x73:
                return read_enc_string();
            case 0x01:
            case 0x1B:{
                          auto o = in.read<int32_t>() + offset;
                          auto p = in.tell();
                          in.seek(o);
                          auto s = read_enc_string();
                          in.seek(p);
                          return s;
            }
            default:
                throw std::runtime_error("Unknown property string type: " + std::to_string(a));
            }
        }
        void deduce_key() {
            auto len = in.read<int8_t>();
            if (len >= 0)
                throw std::runtime_error("I give up");
            auto slen = static_cast<strsize_t>(len == -128 ? in.read<int32_t>() : -len);
            u8key = nullptr;
            for (auto key : keys) {
                auto os = reinterpret_cast<char8_t const *>(in.offset);
                uint8_t mask = 0xAA;
                auto k = reinterpret_cast<char8_t const *>(key);
                bool valid = true;
                for (auto i = 0u; i < slen; ++i, ++mask) {
                    auto c = static_cast<char8_t>(os[i] ^ k[i] ^ mask);
                    if (c < 0x20 || c >= 0x80)
                        valid = false;
                }
                if (valid) {
                    u8key = reinterpret_cast<char8_t const *>(key);
                    u16key = reinterpret_cast<char16_t const *>(key);
                }
            }
            if (!u8key)
                throw std::runtime_error("Failed to identify the locale");
            in.skip(slen);
        }
        void sort_nodes(id_t first, id_t count) {
            std::sort(nodes.begin() + first, nodes.begin() + first + count,
                [this](node & n1, node & n2) {
                if (&n1 == &n2)
                    return false;
                auto & s1 = strings[n1.name];
                auto & s2 = strings[n2.name];
                auto n = strncmp(s1.data, s2.data, std::min(s1.size, s2.size));
                if (n < 0)
                    return true;
                if (n > 0)
                    return false;
                if (s1.size < s2.size)
                    return true;
                if (s1.size > s2.size)
                    return false;
                throw std::runtime_error("Identical strings. This is baaaaaaaaaaaaaad");
            });
        }
        void find_uols(id_t uol_node) {
            auto & n = nodes[uol_node];
            if (n.data_type == node::type::uol) {
                uol_path.push_back(uol_node);
                uols.push_back(uol_path);
                uol_path.pop_back();
            } else if (n.num != 0) {
                uol_path.push_back(uol_node);
                for (auto i = 0u; i < n.num; ++i)
                    find_uols(n.children + i);
                uol_path.pop_back();
            }
        }
        id_t get_child(id_t parent_node, string str) {
            if (parent_node == 0)
                return 0;
            auto & n = nodes[parent_node];
            auto it = std::lower_bound(nodes.begin() + n.children,
                nodes.begin() + n.children + n.num, str,
                [this](node const & n, string s) {
                auto & sn = strings[n.name];
                auto r = strncmp(sn.data, s.data, std::min(sn.size, s.size));
                return r < 0 || r == 0 && sn.size < s.size;
            });
            if (it == nodes.begin() + n.children + n.num)
                return 0;
            return static_cast<id_t>(it - nodes.begin());
        }
        bool resolve_uol(std::vector<id_t> uol) {
            auto & n = nodes[uol.back()];
            uol.pop_back();
            if (n.data_type != node::type::uol)
                throw std::runtime_error("Welp. I failed.");
            auto & s = strings[n.data.string];
            auto b = 0u;
            for (auto i = 0u; i < s.size; ++i)
            if (s.data[i] == '/') {
                if (i - b == 2 && strncmp(s.data + b, "..", 2) == 0)
                    uol.pop_back();
                else
                    uol.push_back(get_child(uol.back(), {s.data + b, static_cast<strsize_t>(i - b)}));
                b = ++i;
            }
            uol.push_back(get_child(uol.back(), {s.data + b, static_cast<strsize_t>(s.size - b)}));
            if (uol.back() == 0)
                return false;
            auto & nr = nodes[uol.back()];
            if (nr.data_type == node::type::uol)
                return false;
            n.data_type = nr.data_type;
            n.children = nr.children;
            n.num = nr.num;
            n.data.integer = nr.data.integer;
            return true;
        }
        void uol_fail(std::vector<id_t> & uol) {
            nodes[uol.back()].data_type = node::type::none;
        }
        void directory(id_t dir_node) {
            std::vector<id_t> directories;
            auto & n = nodes[dir_node];
            auto count = static_cast<id_t>(in.read_cint());
            auto ni = static_cast<id_t>(nodes.size());
            n.num = static_cast<uint16_t>(count);
            n.children = ni;
            nodes.resize(ni + count);
            for (auto i = 0u; i < count; ++i) {
                auto & nn = nodes[ni + i];
                auto type = in.read<uint8_t>();
                switch (type) {
                case 1:
                    throw std::runtime_error("Found the elusive type 1 directory");
                case 2:{
                           auto s = in.read<int32_t>();
                           auto p = in.tell();
                           in.seek(file_start + s);
                           type = in.read<uint8_t>();
                           nn.name = read_enc_string();
                           in.seek(p);
                           break;
                }
                case 3:
                case 4:
                    nn.name = read_enc_string();
                    break;
                default:
                    throw std::runtime_error("Unknown directory type");
                }
                auto size = in.read_cint();
                if (size < 0)
                    throw std::runtime_error("Directory/img has invalid size!");
                in.read_cint();//Offset that nobody cares about
                in.skip(4);//Checksum that nobody cares about
                if (type == 3)
                    directories.push_back(ni + i);
                else if (type == 4)
                    imgs.emplace_back(ni + i, size);
                else
                    throw std::runtime_error("Unknown type 2 directory");
            }
            for (auto it : directories)
                directory(it);
            nodes_to_sort.emplace_back(ni, count);
        }
        void extended_property(id_t prop_node, size_t offset) {
            auto & n = nodes[prop_node];
            auto & st = strings[read_prop_string(offset)];
            if (!strncmp(st.data, "Property", st.size)) {
                in.skip(2);
                sub_property(prop_node, offset);
            } else if (!strncmp(st.data, "Canvas", st.size)) {
                in.skip(1);
                if (in.read<uint8_t>() == 1) {
                    in.skip(2);
                    sub_property(prop_node, offset);
                }
                //Have to recalculate n, because address may have changed
                //because sub_property may increase the size of nodes
                auto & n = nodes[prop_node];
                n.data_type = node::type::bitmap;
                n.data.bitmap.id = static_cast<uint32_t>(bitmaps.size());
                bitmaps.push_back(in.tell());
                n.data.bitmap.width = static_cast<uint16_t>(in.read_cint());
                n.data.bitmap.height = static_cast<uint16_t>(in.read_cint());
            } else if (!strncmp(st.data, "Shape2D#Vector2D", st.size)) {
                n.data_type = node::type::vector;
                n.data.vector[0] = in.read_cint();
                n.data.vector[1] = in.read_cint();
            } else if (!strncmp(st.data, "Shape2D#Convex2D", st.size)) {
                auto count = static_cast<id_t>(in.read_cint());
                auto ni = static_cast<id_t>(nodes.size());
                n.num = static_cast<uint16_t>(count);
                n.children = ni;
                nodes.resize(nodes.size() + count);
                for (auto i = 0u; i < count; ++i) {
                    auto & nn = nodes[ni + i];
                    auto es = std::to_string(i);
                    nn.name = add_string(es.c_str(), static_cast<strsize_t>(es.size()));
                    extended_property(ni, offset);
                }
                nodes_to_sort.emplace_back(ni, count);
            } else if (!strncmp(st.data, "Sound_DX8", st.size)) {
                n.data_type = node::type::audio;
                n.data.audio.id = static_cast<uint32_t>(sounds.size());
                sounds.push_back(in.tell());
                in.skip(1);//Always 0
                n.data.audio.length = static_cast<uint32_t>(in.read_cint());
            } else if (!strncmp(st.data, "UOL", st.size)) {
                in.skip(1);
                n.data_type = node::type::uol;
                n.data.string = read_prop_string(offset);
            } else throw std::runtime_error {"Unknown sub property type: " + std::string {st.data, st.size}};
        }
        void sub_property(id_t prop_node, size_t offset) {
            auto & n = nodes[prop_node];
            auto count = static_cast<id_t>(in.read_cint());
            auto ni = static_cast<id_t>(nodes.size());
            n.num = static_cast<uint16_t>(count);
            n.children = ni;
            nodes.resize(nodes.size() + count);
            for (auto i = 0u; i < count; ++i) {
                auto & nn = nodes[ni + i];
                nn.name = read_prop_string(offset);
                auto type = in.read<uint8_t>();
                switch (type) {
                case 0x00://Turning null nodes into integers with an id. Useful for zmap.img
                    nn.data_type = node::type::integer;
                    nn.data.integer = i;
                    break;
                case 0x0B://Todo - Check if 0x0B really is a 16bit int
                case 0x02:
                    nn.data_type = node::type::integer;
                    nn.data.integer = in.read<uint16_t>();
                    break;
                case 0x03:
                    nn.data_type = node::type::integer;
                    nn.data.integer = in.read_cint();
                    break;
                case 0x04:
                    nn.data_type = node::type::real;
                    nn.data.real = in.read<uint8_t>() == 0x80 ? in.read<float>() : 0.;
                    break;
                case 0x05:
                    nn.data_type = node::type::real;
                    nn.data.real = in.read<double>();
                    break;
                case 0x08:
                    nn.data_type = node::type::string;
                    nn.data.string = read_prop_string(offset);
                    break;
                case 0x09:{
                              auto p = in.read<int32_t>() + in.tell();
                              extended_property(ni + i, offset);
                              in.seek(p);
                              break;
                }
                default:
                    throw std::runtime_error("Unknown sub property type: " + std::to_string(type));
                }
            }
            nodes_to_sort.emplace_back(ni, count);
        }
        void img(id_t img_node, int32_t size) {
            auto p = in.tell();
            in.skip(1);
            deduce_key();
            in.skip(2);
            sub_property(img_node, p);
            in.seek(p + size);
        }
        wztonx(std::string filename) {
            in.open(filename);
            filename.erase(filename.find_last_of('.')).append(".nx");
            auto magic = in.read<uint32_t>();
            if (magic != 0x31474B50)
                throw std::runtime_error("Not a valid WZ file");
            in.skip(8);
            file_start = in.read<uint32_t>();
            //Just skip the copyright string
            in.seek(file_start + 2);
            in.read_cint();
            in.skip(1);
            deduce_key();
            in.seek(file_start + 2);
            add_string("", 0);
            std::cout << "Opened file" << std::endl;
            directory(0);
            std::cout << "Parsed directories" << std::endl;
            for (auto & it : imgs)
                img(it.first, it.second);
            std::cout << "Parsed images" << std::endl;
            for (auto const & n : nodes_to_sort)
                sort_nodes(n.first, n.second);
            find_uols(0);
            for (;;) {
                auto it = std::remove_if(uols.begin(), uols.end(),
                    [this](std::vector<id_t> const & v) {
                    return resolve_uol(v);
                });
                if (it == uols.begin() || it == uols.end())
                    break;
                uols.erase(it, uols.end());
            }
            for (auto & it : uols)
                uol_fail(it);
            std::cout << "Node cleanup finished" << std::endl;
            auto node_offset = 52UL;
            node_offset += 0x10 - (node_offset & 0xf);
            auto string_table_offset = node_offset + nodes.size() * 20;
            string_table_offset += 0x10 - (string_table_offset & 0xf);
            auto string_offset = string_table_offset + strings.size() * 8;
            string_offset += 0x10 - (string_offset & 0xf);
            auto bitmap_table_offset = string_offset + strings.size() * 2;
            for (auto const & s : strings)
                bitmap_table_offset += s.size;
            bitmap_table_offset += 0x10 - (bitmap_table_offset & 0xf);
            out.open(filename, bitmap_table_offset);
            out.seek(0);
            out.write<uint32_t>(0x34474B50);
            out.write<uint32_t>(static_cast<uint32_t>(nodes.size()));
            out.write<uint64_t>(node_offset);
            out.write<uint32_t>(static_cast<uint32_t>(strings.size()));
            out.write<uint64_t>(string_table_offset);
            //No bitmap or audio support yet
            out.write<uint32_t>(0);
            out.write<uint64_t>(0);
            out.write<uint32_t>(0);
            out.write<uint64_t>(0);
            std::cout << "Opened output" << std::endl;
            out.seek(node_offset);
            out.write(nodes.data(), nodes.size() * 20);
            std::cout << "Wrote nodes" << std::endl;
            out.seek(string_table_offset);
            auto next_str = string_offset;
            for (auto const & s : strings) {
                out.write<uint64_t>(next_str);
                next_str += s.size + 2;
            }
            out.seek(string_offset);
            for (auto const & s : strings) {
                out.write<uint16_t>(s.size);
                out.write(s.data, s.size);
            }
            std::cout << "Wrote strings" << std::endl;
            std::cout << "Done" << std::endl;
        }
    };
}
int main(int argc, char ** argv) {
    auto a = std::chrono::high_resolution_clock::now();
    nl::wztonx(argc > 1 ? argv[1] : "Data.wz");
    auto b = std::chrono::high_resolution_clock::now();
    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count() << " ms" << std::endl;
}
