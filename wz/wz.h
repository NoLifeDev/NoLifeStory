///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include <string>
#include <filesystem>
#include <cstdint>
#include <vector>
#include "sprite.h"

namespace WZ {
    class Node {
    public:
        class Data;
        Node();
        Node(const Node&);
        Node(Data*);
        Node(Data&);
        Node operator= (const Node&);
        Node operator[] (std::string) const;
        Node operator[] (const char[]) const;
        Node operator[] (int) const;
        Node operator[] (const Node&) const;
        Node g(std::string, int n);
        std::string Name() const;
        void InitTop(std::string);
        void Assign(const Node&);
        operator bool() const;
        operator std::string() const;
        operator double() const;
        operator int() const;
        //operator Sprite() const;
        //operator Sound() const;
        void Set(std::string);
        void SetUOL(std::string);
        void Set(double);
        void Set(int);
        void Set(class Img*);
        //void Set(const class Sound&);
        //void Set(const class Sprite&);
        void Resolve();
        void Reserve(int);
    private:
        Data* data;
    };
    extern Node WZ;
    extern uint16_t Version;
    extern std::vector<std::tr2::sys::path> Paths;
    void Init();
}
