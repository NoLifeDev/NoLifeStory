///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include "../wz/wz.h"
#include <iostream>
#include <thread>
#include <set>
using namespace std;
using namespace chrono;
using namespace WZ;

int main() {
    freopen("log.txt", "w", stdout);
    
    AddPath("");
    AddPath("C:/Nexon/MapleGlobal 109/");
    high_resolution_clock clock;
    high_resolution_clock::time_point start = clock.now();
    Init(true);
    high_resolution_clock::time_point end = clock.now();
    for (Node n : Base["Etc"]["Curse"]) {
        cout << n.Name() << ": " << (string)n << endl;
    }
    cout << "Time taken: " << duration_cast<milliseconds>(end-start).count() << " ms" << endl;
    
}