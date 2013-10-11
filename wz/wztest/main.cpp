///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include "../wz/wz.h"
#include <iostream>
#include <set>
#include <Windows.h>
#include <Psapi.h>
using namespace std;
using namespace WZ;

void recurse(Node n) {
    for (Node nn : n) recurse(nn);
}
int main() {
    LARGE_INTEGER freq, last, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last);
    AddPath("");
    Init();
    QueryPerformanceCounter(&now);
    cout << "Load: " << 1000000*(now.QuadPart-last.QuadPart)/freq.QuadPart << " microseconds" << endl;
    QueryPerformanceCounter(&last);
    for (int n = 0; n < 1000000; ++n) {
        Node nn = Base["Effect"]["BasicEff.img"]["LevelUp"]["5"]["origin"];
    }
    QueryPerformanceCounter(&now);
    cout << "Access: " << 1000*(now.QuadPart-last.QuadPart)/freq.QuadPart << " milliseconds" << endl;
    QueryPerformanceCounter(&last);
    recurse(Base);
    QueryPerformanceCounter(&now);
    cout << "Recursive Parse: " << 1000*(now.QuadPart-last.QuadPart)/freq.QuadPart << " milliseconds" << endl;
    QueryPerformanceCounter(&last);
    recurse(Base);
    QueryPerformanceCounter(&now);
    cout << "Recurse: " << 1000*(now.QuadPart-last.QuadPart)/freq.QuadPart << " milliseconds" << endl;
    PROCESS_MEMORY_COUNTERS proc;
    GetProcessMemoryInfo(GetCurrentProcess(), &proc, sizeof(proc));
    cout << "Memory usage: " << proc.PeakPagefileUsage/1000 << " kilobytes" << endl;
}