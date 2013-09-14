//////////////////////////////////////////////////////////////////////////////
// NoLifeNx - Part of the NoLifeStory project                               //
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

#include "file.hpp"
#include "node.hpp"
#ifdef _WIN32
#  include <Windows.h>
#else
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <sys/fcntl.h>
#  include <sys/mman.h>
#  include <unistd.h>
#endif
#include <stdexcept>

namespace nl {
    file::file(std::string name) {
#ifdef _WIN32
        m_file = CreateFileA(name.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, nullptr);
        if (m_file == INVALID_HANDLE_VALUE) throw std::runtime_error {"Failed to open file " + name};
        m_map = CreateFileMappingA(m_file, 0, PAGE_READONLY, 0, 0, nullptr);
        if (!m_map) throw std::runtime_error {"Failed to create file mapping of file " + name};
        m_base = MapViewOfFile(m_map, FILE_MAP_READ, 0, 0, 0);
        if (!m_base) throw std::runtime_error {"Failed to map view of file " + name};
#else
        m_file = open(name.c_str(), O_RDONLY);
        if (m_file == -1) throw std::runtime_error {"Failed to open file " + name};
        struct stat finfo;
        if (fstat(m_file, &finfo) == -1) throw std::runtime_error {"Failed to obtain file information of file " + name};
        m_size = finfo.st_size;
        m_base = mmap(nullptr, m_size, PROT_READ, MAP_SHARED, m_file, 0);
        if (reinterpret_cast<intptr_t>(m_base) == -1) throw std::runtime_error {"Failed to create memory mapping of file " + name};
#endif
        m_header = reinterpret_cast<header const *>(m_base);
        if (m_header->magic != 0x34474B50) throw std::runtime_error {name + " is not a PKG4 NX file"};
        m_node_table = reinterpret_cast<node::data const *>(reinterpret_cast<char const *>(m_base) + m_header->node_offset);
        m_string_table = reinterpret_cast<uint64_t const *>(reinterpret_cast<char const *>(m_base) + m_header->string_offset);
        m_bitmap_table = reinterpret_cast<uint64_t const *>(reinterpret_cast<char const *>(m_base) + m_header->bitmap_offset);
        m_audio_table = reinterpret_cast<uint64_t const *>(reinterpret_cast<char const *>(m_base) + m_header->audio_offset);
    }
    file::~file() {
#ifdef _WIN32
        UnmapViewOfFile(m_base);
        CloseHandle(m_map);
        CloseHandle(m_file);
#else
        munmap(const_cast<void *>(m_base), m_size);
        close(m_file);
#endif
    }
    node file::root() const {
        return {m_node_table, this};
    }
    file::operator node() const {
        return {m_node_table, this};
    }
    uint32_t file::string_count() const {
        return m_header->string_count;
    }
    uint32_t file::bitmap_count() const {
        return m_header->bitmap_count;
    }
    uint32_t file::audio_count() const {
        return m_header->audio_count;
    }
    uint32_t file::node_count() const {
        return m_header->node_count;
    }
    std::string file::get_string(uint32_t i) const {
        char const * const s {reinterpret_cast<char const *>(m_base) + m_string_table[i]};
        return {s + 2, *reinterpret_cast<uint16_t const *>(s)};
    }
}
