//////////////////////////////////////////////////////////////////////////
// Copyright 2012 Peter Atechian (Retep998)                             //
//////////////////////////////////////////////////////////////////////////
// This file is part of the NoLifeStory project.                        //
//                                                                      //
// NoLifeStory is free software: you can redistribute it and/or modify  //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// NoLifeStory is distributed in the hope that it will be useful,       //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with NoLifeStory.  If not, see <http://www.gnu.org/licenses/>. //
//////////////////////////////////////////////////////////////////////////
#include "NX.h"
#ifdef _WIN32
#include <Windows.h>
#include <Psapi.h>
#elif defined __linux__
#include <time.h>
#endif
#include <iostream>
#include <thread>
#include <functional>
using namespace std;
using namespace NL;

#ifdef _WIN32
LARGE_INTEGER freq;
#endif

void Test(function<void()> f, const char* name, size_t n) {
#ifdef _WIN32
    LARGE_INTEGER last, now;
    size_t ticks;
#elif defined __linux__
    timespec last, now;
    size_t time;
#endif
    for (size_t i = n; i > 0; --i) {
#ifdef _WIN32
        QueryPerformanceCounter(&last);
#elif defined __linux__
        clock_gettime(CLOCK_MONOTONIC, &last);
#endif
        f();
#ifdef _WIN32
        QueryPerformanceCounter(&now);
        size_t t = now.QuadPart - last.QuadPart;
        if (i == n || t < ticks) {
            ticks = t;
        }
#elif defined __linux__
        clock_gettime(CLOCK_MONOTONIC, &now);
        size_t t = now.tv_nsec - last.tv_nsec;
        if (i == n || t < time) {
            time = t;
        }
#endif
    }
#ifdef _WIN32
    size_t s = ticks/freq.QuadPart;
    size_t ms = 1000*ticks/freq.QuadPart%1000;
    size_t us = 1000000*ticks/freq.QuadPart%1000;
    size_t ns = 1000000000*ticks/freq.QuadPart%1000;
    cout << name << ": " << s << "s " << ms << "ms " << us << "us " << ns << "ns " << ticks << " ticks" << endl;
#elif defined __linux__
    size_t s = time/1000000000;
    size_t ms = time/1000000;
    size_t us = time/1000;
    size_t ns = time;
    cout << name << ": " << s << "s " << ms << "ms " << us << "us " << ns << "ns " << endl;
#endif
}

int main() {
#ifdef _WIN32
    QueryPerformanceFrequency(&freq);
#endif
    Test([&](){Load("Data.nx");}, "Load", 1000);
    Node nn;
    Test([&](){for (size_t i = 1000000; i > 0; --i) nn = Base["Effect"]["BasicEff.img"]["LevelUp"]["5"]["origin"];}, "Access", 10);
    volatile int x = nn.X();
    Test([&](){NL::Recurse(Base.d);}, "Recursion", 10);
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS proc;
    GetProcessMemoryInfo(GetCurrentProcess(), &proc, sizeof(proc));
    cout << "Memory usage: " << proc.PeakPagefileUsage/1000 << "kb " << proc.PeakPagefileUsage%1000 << "b " << endl;
#endif
}
