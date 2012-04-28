///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include "wzmain.h"

namespace WZ {
    class Node::Data {
    public:
        Data()
            :parent(), name(), num(0), children(0) {}
        uint64_t value;
        Node parent;
        string name;
        Data* children;
        uint16_t num;
        enum class Type : uint8_t {
            none,
            real,
            string,
            uol
        } type;
    private:
        Data(const Node::Data&);
    };

    Node::Node() : data(nullptr) {}
    
    Node::Node(Data* data) : data(data) {}

    Node::Node(Data& data) : data(&data) {}

    Node::Node(const Node& other) : data(other.data) {}

    Node Node::operator= (const Node& other) {
        return data = other.data;
    }

    Node Node::operator[] (string key) const {
        if (!data) return Node();
        if (key == "..") return data->parent;
        if (key == ".") return *this;
        if (!data->children) return Node();
        auto it = find_if(data->children, data->children+data->num, [&](const Data& d){return d.name == key;});
        if (it == data->children+data->num) return Node();
        return *it;
    }

    Node Node::operator[] (const char key[]) const {
        return (*this)[(string)key];
    }

    Node Node::operator[] (int key) const {
        return (*this)[to_string(key)];
    }
    Node Node::operator[] (const Node& key) const {
        return (*this)[(string)key];
    }

    Node Node::g(string key, int n) {
        data->children[n].name = key;
        data->children[n].parent = *this;
        return data->children[n];
    }

    string Node::Name() const {
        if (!data) return string();
        return data->name;
    }

    void Node::InitTop(string s) {
        data = new Data();
        data->name = s;
        data->parent = *this;
    }

    void Node::Assign(const Node& other) {
        data = other.data;
    }

    Node::operator bool() const {
        return (bool)data;
    }

    Node::operator string() const {
        if (!data) return string();
        switch (data->type) {
        case Data::Type::real:
            return to_string(*(double*)&data->value);
        case Data::Type::string:
            return string((char*)data->value); 
        default:
            return string();
            //return *(double*)&data->value;
        }
    }

    Node::operator double() const {
        if (!data) return 0;
        if (data->type == Data::Type::real) return *(double*)&data->value;
        else return 0;
    }

    Node::operator int() const {
        if (!data) return 0;
        if (data->type == Data::Type::real) return *(double*)&data->value;
        else return 0;
    }

    /*Node::operator Sprite() const {
        if (!data) return Sprite();
        return data->sprite;
    }

    Node::operator Sound() const {
        if (!data) return Sound();
        return data->sound;
    }*/

    void Node::Set(const string v) {
        void* ptr = malloc(v.length()+1);
        memcpy(ptr, v.c_str(), v.length()+1);
        data->type = Data::Type::string;
    }

    void Node::SetUOL(const string v) {
        void* ptr = malloc(v.length()+1);
        memcpy(ptr, v.c_str(), v.length()+1);
        data->type = Data::Type::uol;
    }

    void Node::Set(const double v) {
        *(double*)&data->value = v;
        data->type = Data::Type::real;
    }

    void Node::Set(const int v) {
        *(double*)&data->value = v;
        data->type = Data::Type::real;
    }

    /*void Node::Set(const Sprite& v) {
        data->sprite = v;
    }

    void Node::Set(const Sound& v) {
        data->sound = v;
    }*/

    void Node::Resolve() {
        if (!data) return;
        if (data->type == Data::Type::uol) {
            string s = string((char*)data->value); ;
	        vector<string> parts;
            string str;
	        for (char c : s) {
		        if (c == '/') {
                    parts.push_back(str);
                    str.clear();
                } else str += c;
	        }
            parts.push_back(str);
            Node nn = data->parent;
            for (string str : parts) {
                if (!nn) break;
                nn = nn[str];
            }
            if (nn) nn.data = data;
        } else {
            for_each(data->children, data->children+data->num, [&](Data& d){Node(d).Resolve();});
        }
    }
    
    void Node::Reserve(int n) {
        data->children = new Data[n];
    }
}