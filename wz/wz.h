///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include <filesystem>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <codecvt>
#include <sstream>
#include <iostream>
using namespace std;
using namespace std::tr1;
using namespace std::tr2;
using namespace std::tr2::sys;

static_assert(sizeof(wchar_t) == 2, "");

namespace WZ {
    class Node {
    public:
        Node();
        Node(const Node&);
        Node& operator= (const Node&);
        Node operator[] (const string) const;
        Node operator[] (const char[]) const;
        Node operator[] (const int) const;
        Node operator[] (const Node) const;
        Node g(string);
        map<string, Node>::const_iterator begin() const;
        map<string, Node>::const_iterator end() const;
        map<string, Node>::const_reverse_iterator rbegin() const;
        map<string, Node>::const_reverse_iterator rend() const;
        string Name() const;
        void InitTop(const string);
        void Assign(const Node);
        operator bool() const;
        operator string() const;
        operator double() const;
        operator int() const;
        //operator Sprite() const;
        //operator Sound() const;
        void Set(const string);
        void Set(const double);
        void Set(const int);
        void Set(class Img*);
        //void Set(const class Sound&);
        //void Set(const class Sprite&);
        void Recurse();
    private:
        Node(const Node&, string);
        class Data;
        Data* data;
    };
    extern Node WZ;
    extern uint16_t Version;
    extern vector<path> Paths;
    void Init();
}
