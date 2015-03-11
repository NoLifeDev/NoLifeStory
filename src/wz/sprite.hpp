///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

class Sprite {
    Sprite() : data(nullptr) {}
    Sprite(const Sprite & other) : data(other.data) {}
    class Data;

private:
    Data * data;
};