//////////////////////////////////////////////////////////////////////////////
// NoLifeNx - Part of the NoLifeStory project                               //
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

#pragma once
#include <cstdint>
#include <string>

namespace nl {
    class file {
    public:
        //Used to construct an nx file from a filename
        //Multiple file objects can be created from the same filename without problem
        //and the resulting nodes are interchangeable
        file(std::string name);
        //Upon being destroyed all nodes that originated from this file become invalid
        //and may error or crash on access
        ~file();
        //Obtains the root node from which all other nodes may be accessed
        class node root() const;
        //An alternate way to obtain the root node that saves some typing
        operator node() const;
        //Returns the number of strings in the file
        uint32_t string_count() const;
        //Returns the number of bitmaps in the file
        uint32_t bitmap_count() const;
        //Returns the number of audios in the file
        uint32_t audio_count() const;
        //Returns the number of nodes in the file
        uint32_t node_count() const;
        std::string get_string(uint32_t) const;
    private:
#pragma pack(push, 1)
        struct header {
            uint32_t const magic;
            uint32_t const node_count;
            uint64_t const node_offset;
            uint32_t const string_count;
            uint64_t const string_offset;
            uint32_t const bitmap_count;
            uint64_t const bitmap_offset;
            uint32_t const audio_count;
            uint64_t const audio_offset;
        };
#pragma pack(pop)
        file(const file &);//Todo: Replace with = delete once VS has support for it.
        file & operator=(const file &);//Todo: Replace with = delete once VS has support for it.
        void const * m_base;
        struct node_data const * m_node_table;
        uint64_t const * m_string_table;
        uint64_t const * m_bitmap_table;
        uint64_t const * m_audio_table;
        header const * m_header;
#ifdef _WIN32
        void * m_file;
        void * m_map;
#else
        int m_file;
        size_t m_size;
#endif
        friend class node;
        friend class bitmap;
        friend class audio;
    };
}
