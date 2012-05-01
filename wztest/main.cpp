///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include "../wz/wz.h"
#include <iostream>
#include <thread>
using namespace std;

int main() {
    chrono::high_resolution_clock clock;
    WZ::AddPath("");
    //WZ::AddPath("C:/Nexon/MapleStory 40b/");
    WZ::AddPath("C:/Nexon/MapleStory 109/");
    WZ::AddPath("C:/Nexon/MapleStory/");
    WZ::AddPath("D:/Program Files (x86)/MapleStory_gms_109/");
    chrono::high_resolution_clock::time_point start = clock.now();
    WZ::Init(false);
    chrono::high_resolution_clock::time_point end = clock.now();
    cout << "Time taken: " << chrono::duration_cast<chrono::milliseconds>(end-start).count() << " ms" << endl;
}