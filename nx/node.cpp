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
#include "node.hpp"
#include "file.hpp"
#include "bitmap.hpp"
#include "audio.hpp"
#include <cstring>
#include <stdexcept>
namespace nl {
#ifdef _MSC_VER
    node::node() : m_data {nullptr}, m_file {nullptr} {}
    node::node(node const & o) : m_data {o.m_data}, m_file {o.m_file} {}
    node::node(data const * d, file const * f) : m_data {d}, m_file {f} {}
    node & node::operator=(node const & o) {
        return m_data = o.m_data, m_file = o.m_file, *this;
    }
#endif
    node node::begin() const {
        return {m_data ? m_file->m_node_table + m_data->children : nullptr, m_file};
    }
    node node::end() const {
        return {m_data ? m_file->m_node_table + m_data->children + m_data->num : nullptr, m_file};
    }
    node node::operator*() const {
        return *this;
    }
    node & node::operator++() {
        return ++m_data, *this;
    }
    node node::operator++(int) {
        return {m_data++, m_file};
    }
    bool node::operator==(node const & o) const {
        return m_data == o.m_data;
    }
    bool node::operator!=(node const & o) const {
        return m_data != o.m_data;
    }
    std::string operator+(std::string s, node n) {
        return std::move(s) + n.get_string();
    }
    std::string operator+(char const * s, node n) {
        return s + n.get_string();
    }
    std::string node::operator+(std::string const & s) const {
        return get_string() + s;
    }
    std::string node::operator+(char const * s) const {
        return get_string() + s;
    }
    node node::operator[](unsigned int n) const {
        return operator[](std::to_string(n));
    }
    node node::operator[](signed int n) const {
        return operator[](std::to_string(n));
    }
    node node::operator[](unsigned long n) const {
        return operator[](std::to_string(n));
    }
    node node::operator[](signed long n) const {
        return operator[](std::to_string(n));
    }
    node node::operator[](unsigned long long n) const {
        return operator[](std::to_string(n));
    }
    node node::operator[](signed long long n) const {
        return operator[](std::to_string(n));
    }
    node node::operator[](std::string const & o) const {
        return get_child(o.c_str(), o.length());
    }
    node node::operator[](char const * o) const {
        return get_child(o, std::strlen(o));
    }
    node node::operator[](node const & o) const {
        return operator[](o.get_string());
    }
    node node::operator[](std::pair<char const *, size_t> const & o) const {
        return get_child(o.first, o.second);
    }
    node::operator unsigned char() const {
        return static_cast<unsigned char>(get_integer());
    }
    node::operator signed char() const {
        return static_cast<signed char>(get_integer());
    }
    node::operator unsigned short() const {
        return static_cast<unsigned short>(get_integer());
    }
    node::operator signed short() const {
        return static_cast<signed short>(get_integer());
    }
    node::operator unsigned int() const {
        return static_cast<unsigned int>(get_integer());
    }
    node::operator signed int() const {
        return static_cast<signed int>(get_integer());
    }
    node::operator unsigned long() const {
        return static_cast<unsigned long>(get_integer());
    }
    node::operator signed long() const {
        return static_cast<signed long>(get_integer());
    }
    node::operator unsigned long long() const {
        return static_cast<unsigned long long>(get_integer());
    }
    node::operator signed long long() const {
        return static_cast<signed long long>(get_integer());
    }
    node::operator float() const {
        return static_cast<float>(get_real());
    }
    node::operator double() const {
        return static_cast<double>(get_real());
    }
    node::operator long double() const {
        return static_cast<long double>(get_real());
    }
    node::operator std::string() const {
        return get_string();
    }
    node::operator std::pair<int32_t, int32_t>() const {
        return get_vector();
    }
    node::operator bitmap() const {
        return get_bitmap();
    }
    node::operator audio() const {
        return get_audio();
    }
    node::operator bool() const {
        return m_data ? true : false;
    }
    int64_t node::get_integer() const {
        return get_integer(0);
    }
    int64_t node::get_integer(int64_t def) const {
        if (m_data) switch (m_data->type) {
        case type::none: return def;
        case type::integer: return to_integer();
        case type::real: return static_cast<int64_t>(to_real());
        case type::string: return std::stoll(to_string());
        case type::vector: return def;
        case type::bitmap: return def;
        case type::audio: return def;
        default: throw std::runtime_error {"Unknown node type"};
        }
        return def;
    }
    double node::get_real() const {
        return get_real(0);
    }
    double node::get_real(double def) const {
        if (m_data) switch (m_data->type) {
        case type::none: return def;
        case type::integer: return static_cast<double>(to_integer());
        case type::real: return to_real();
        case type::string: return std::stod(to_string());
        case type::vector: return def;
        case type::bitmap: return def;
        case type::audio: return def;
        default: throw std::runtime_error {"Unknown node type"};
        }
        return def;
    }
    std::string node::get_string() const {
        if (m_data) switch (m_data->type) {
        case type::none: return std::string {};
        case type::integer: return std::to_string(to_integer());
        case type::real: return std::to_string(to_real());
        case type::string: return to_string();
        case type::vector: return "(" + std::to_string(m_data->vector[0]) + ", " + std::to_string(m_data->vector[1]) + ")";
        case type::bitmap: return "bitmap";
        case type::audio: return "audio";
        default: throw std::runtime_error {"Unknown node type"};
        }
        return std::string();
    }
    std::pair<int32_t, int32_t> node::get_vector() const {
        return m_data && m_data->type == type::vector ? to_vector() : std::pair<int32_t, int32_t> {0, 0};
    }
    bitmap node::get_bitmap() const {
        return m_data && m_data->type == type::bitmap && m_file->m_header->bitmap_count ? to_bitmap() : bitmap {nullptr, 0, 0};
    }
    audio node::get_audio() const {
        return m_data && m_data->type == type::audio && m_file->m_header->audio_count ? to_audio() : audio {nullptr, 0};
    }
    bool node::get_bool() const {
        return m_data && m_data->type == type::integer && to_integer() ? true : false;
    }
    bool node::get_bool(bool def) const {
        return m_data && m_data->type == type::integer ? to_integer() ? true : false : def;
    }
    int32_t node::x() const {
        return m_data && m_data->type == type::vector ? m_data->vector[0] : 0;
    }
    int32_t node::y() const {
        return m_data && m_data->type == type::vector ? m_data->vector[1] : 0;
    }
    std::string node::name() const {
        return !m_data ? std::string {} : m_file->get_string(m_data->name);
    }
    std::pair<char const *, size_t> node::name_fast() const {
        if (!m_data) return {nullptr, 0};
        char const * s {reinterpret_cast<char const *>(m_file->m_base) + m_file->m_string_table[m_data->name]};
        return {s + 2, *reinterpret_cast<uint16_t const *>(s)};
    }
    size_t node::size() const {
        return m_data ? m_data->num : 0U;
    }
    node::type node::data_type() const {
        return m_data ? m_data->type : type::none;
    }
    node node::get_child(char const * const o, size_t const l) const {
        if (!m_data) return {nullptr, m_file};
        data const * p {m_file->m_node_table + m_data->children};
        size_t n {m_data->num};
        char const * const b {reinterpret_cast<const char *>(m_file->m_base)};
        uint64_t const * const t {m_file->m_string_table};
        for (;;) {
            if (!n) return {nullptr, m_file};
            size_t const n2 {n >> 1};
            data const * const p2 {p + n2};
            char const * const sl {b + t[p2->name]};
            size_t const l1 {*reinterpret_cast<uint16_t const *>(sl)};
            uint8_t const * s {reinterpret_cast<uint8_t const *>(sl + 2)};
            uint8_t const * os {reinterpret_cast<uint8_t const *>(o)};
            bool z {false};
            for (size_t i {l1 < l ? l1 : l}; i; --i, ++s, ++os) {
                if (*s > *os) {
                    n = n2;
                    z = true;
                    break;
                } else if (*s < *os) {
                    p = p2 + 1;
                    n -= n2 + 1;
                    z = true;
                    break;
                }
            }
            if (z) continue;
            else if (l1 < l) p = p2 + 1, n -= n2 + 1;
            else if (l1 > l) n = n2;
            else return {p2, m_file};
        }
    }
    int64_t node::to_integer() const {
        return m_data->ireal;
    }
    double node::to_real() const {
        return m_data->dreal;
    }
    std::string node::to_string() const {
        return m_file->get_string(m_data->string);
    }
    std::pair<int32_t, int32_t> node::to_vector() const {
        return {m_data->vector[0], m_data->vector[1]};
    }
    bitmap node::to_bitmap() const {
        return {reinterpret_cast<char const *>(m_file->m_base) + m_file->m_bitmap_table[m_data->bitmap.index], m_data->bitmap.width, m_data->bitmap.height};
    }
    audio node::to_audio() const {
        return {reinterpret_cast<char const *>(m_file->m_base) + m_file->m_audio_table[m_data->audio.index], m_data->audio.length};
    }
}
