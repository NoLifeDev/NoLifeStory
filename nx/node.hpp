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
#include <string>
#include <cstdint>

namespace nl {
    class bitmap;
    class audio;
    class file;
    class node {
    public:
        //Type of node data
        enum class type : uint16_t {
            none = 0,
            integer = 1,
            real = 2,
            string = 3,
            vector = 4,
            bitmap = 5,
            audio = 6,
        };
        //Internal data structure
        typedef struct node_data data;
        //MSVC is terrible without manual constructors
#ifdef _MSC_VER
        node();
        node(node const &);
        node(data const *, file const *);
        node & operator=(node const &);
#endif
        //These methods are primarily so nodes can be used as iterators and iterated over
        node begin() const;
        node end() const;
        node operator*() const;
        node & operator++();
        node operator++(int);
        bool operator==(node const &) const;
        bool operator!=(node const &) const;
        //This checks whether or not the node points to an actual node
        //Even if the node ends up being a null node, you can still use it safely
        //It'll just fall back to returning default values and more null nodes
        //However, if the file this node was obtained from was deleted
        //then the node becomes invalid and this operator cannot tell you that
        explicit operator bool() const;
        //Used to easily concatenate the string value of a node with another string
        std::string operator+(std::string const &) const;
        std::string operator+(char const *) const;
        //Methods to access the children of the node by name
        //Note that the versions taking integers convert the integer to a string
        //They do not access the children by their integer index
        //If you wish to do that, use somenode.begin() + integer_index
        node operator[](unsigned int) const;
        node operator[](signed int) const;
        node operator[](unsigned long) const;
        node operator[](signed long) const;
        node operator[](unsigned long long) const;
        node operator[](signed long long) const;
        node operator[](std::string const &) const;
        node operator[](char const *) const;
        //This method uses the string value of the node, not the node's name
        node operator[](node const &) const;
        node operator[](std::pair<char const *, size_t> const &) const;
        //Operators to easily cast a node to get the data
        //Allows things like string s = somenode
        //Will automatically cast between data types as needed
        //For example if the node has an integer value but you want a string
        //then the operator will automatically convert the integer to a string
        operator unsigned char() const;
        operator signed char() const;
        operator unsigned short() const;
        operator signed short() const;
        operator unsigned int() const;
        operator signed int() const;
        operator unsigned long() const;
        operator signed long() const;
        operator unsigned long long() const;
        operator signed long long() const;
        operator float() const;
        operator double() const;
        operator long double() const;
        operator std::string() const;
        operator std::pair<int32_t, int32_t>() const;
        operator bitmap() const;
        operator audio() const;
        //Explicitly called versions of all the operators
        //When it takes a parameter, that is used as the default value
        //if a suitable data value cannot be found in the node
        int64_t get_integer() const;
        int64_t get_integer(int64_t) const;
        double get_real() const;
        double get_real(double) const;
        std::string get_string() const;
        std::pair<int32_t, int32_t> get_vector() const;
        class bitmap get_bitmap() const;
        class audio get_audio() const;
        bool get_bool() const;
        bool get_bool(bool) const;
        //Returns the x and y coordinates of the vector data value
        int32_t x() const;
        int32_t y() const;
        //The name of the node
        std::string name() const;
        std::pair<char const *, size_t> name_fast() const;
        //The number of children in the node
        size_t size() const;
        //Gets the type of data contained within the node
        type data_type() const;
        //Internal variables
        //They are only public so that the class may be Plain Old Data
        data const * m_data;
        class file const * m_file;
    private:
        node get_child(char const *, size_t) const;
        int64_t to_integer() const;
        double to_real() const;
        std::string to_string() const;
        std::pair<int32_t, int32_t> to_vector() const;
        class bitmap to_bitmap() const;
        class audio to_audio() const;
        friend class file;
    };
    //More convenience string concatenation operators
    std::string operator+(std::string, node);
    std::string operator+(char const *, node);
    //Internal data structure
#pragma pack(push, 1)
    struct node_data {
        uint32_t const name;
        uint32_t const children;
        uint16_t const num;
        node::type const type;
        union {
            int64_t const ireal;
            double const dreal;
            uint32_t const string;
            int32_t const vector[2];
            struct {
                uint32_t index;
                uint16_t width;
                uint16_t height;
            } const bitmap;
            struct {
                uint32_t index;
                uint32_t length;
            } const audio;
        };
    };
#pragma pack(pop)
}
