///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include "../wz/wz.h"
#include <iostream>
#include <thread>
using namespace std;
using namespace chrono;
using namespace WZ;

int main() {
    high_resolution_clock clock;
    AddPath("");
    AddPath("C:/Nexon/MapleStory/");
    //WZ::AddPath("C:/Nexon/MapleStory 40b/");
    AddPath("C:/Nexon/MapleStory 109/");
    AddPath("D:/Program Files (x86)/MapleStory_gms_109/");
    high_resolution_clock::time_point start = clock.now();
    Init(true);
    high_resolution_clock::time_point end = clock.now();
    for (Node n : Base) {
        cout << n.Name() << endl;
    }
    cout << "Time taken: " << duration_cast<milliseconds>(end-start).count() << " ms" << endl;
    
}