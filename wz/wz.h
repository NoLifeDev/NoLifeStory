///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include <string>
#include <map>
#include <filesystem>
#include <cstdint>
#include <vector>

namespace WZ {
    class Node {
    public:
        Node();
        Node(const Node&);
        Node& operator= (const Node&);
        Node operator[] (const std::string) const;
        Node operator[] (const char[]) const;
        Node operator[] (const int) const;
        Node operator[] (const Node) const;
        Node g(std::string);
        std::map<std::string, Node>::const_iterator begin() const;
        std::map<std::string, Node>::const_iterator end() const;
        std::map<std::string, Node>::const_reverse_iterator rbegin() const;
        std::map<std::string, Node>::const_reverse_iterator rend() const;
        std::string Name() const;
        void InitTop(const std::string);
        void Assign(const Node);
        operator bool() const;
        operator std::string() const;
        operator double() const;
        //operator Sprite() const;
        //operator Sound() const;
        void Set(const std::string);
        void Set(const double);
        void Set(class Img*);
        //void Set(const class Sound&);
        //void Set(const class Sprite&);
        void Recurse();
    private:
        Node(const Node&, std::string);
        class Data;
        Data* data;
    };
    extern Node WZ;
    extern uint16_t Version;
    extern std::vector<std::tr2::sys::path> Paths;
    void Init();
}
