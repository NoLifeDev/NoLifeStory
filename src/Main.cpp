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
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Psapi.h>
#elif defined __linux__
#include <time.h>
#endif
#include <iostream>
#include <functional>
#include <iomanip>

#ifdef _WIN32
void Test(std::function<void()> f, const char * name, size_t n, size_t d) {
    uint64_t ticks = -1;
    for (size_t i = n; i; --i) {
        uint64_t last = __rdtsc();
        for (size_t j = d; j; --j) f();
        uint64_t now = __rdtsc();
        ticks = std::min(now - last, ticks);
    }
    std::cout << std::setw(28) << name << ": " << ticks/d << " cycles" << std::endl;
}
#elif defined __linux__
void Test(std::function<void()> f, const char * name, size_t n) {
    timespec last, now;
    size_t time;
    for (size_t i = n; i > 0; --i) {
        clock_gettime(CLOCK_MONOTONIC, &last);
        f();
        clock_gettime(CLOCK_MONOTONIC, &now);
        size_t t = now.tv_nsec - last.tv_nsec;
        if (i == n || t < time) {
            time = t;
        }
    }
    size_t s = time/1000000000;
    size_t ms = time/1000000;
    size_t us = time/1000;
    size_t ns = time;
    cout << name << ": " << s << "s " << ms << "ms " << us << "us " << ns << "ns " << endl;
}
#endif

void Recurse(NL::Node n) {
    for (NL::Node nn : n) Recurse(nn);
}

int main() {
    Test([&](){delete new NL::File("Data.nx");}, "Load", 1000, 1);
    NL::Node n;
    NL::File file("Data.nx");
    Test([&](){n = file.Base()["Effect"]["BasicEff.img"]["LevelUp"]["5"]["origin"];}, "Access", 1000, 1000);
    volatile int x = n.X();
    Test([&](){Recurse(file.Base());}, "Recursion", 10, 1);
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS proc;
    GetProcessMemoryInfo(GetCurrentProcess(), &proc, sizeof(proc));
    std::cout << std::setw(30) << "PageFaultCount: " << proc.PageFaultCount << " times" << std::endl;
    std::cout << std::setw(30) << "PeakWorkingSetSize: " << proc.PeakWorkingSetSize << " bytes" << std::endl;
    std::cout << std::setw(30) << "WorkingSetSize: " << proc.WorkingSetSize << " bytes" << std::endl;
    std::cout << std::setw(30) << "QuotaPeakPagedPoolUsage: " << proc.QuotaPeakPagedPoolUsage << " bytes" << std::endl;
    std::cout << std::setw(30) << "QuotaPagedPoolUsage: " << proc.QuotaPagedPoolUsage << " bytes" << std::endl;
    std::cout << std::setw(30) << "QuotaPeakNonPagedPoolUsage: " << proc.QuotaPeakNonPagedPoolUsage << " bytes" << std::endl;
    std::cout << std::setw(30) << "QuotaNonPagedPoolUsage: " << proc.QuotaNonPagedPoolUsage << " bytes" << std::endl;
    std::cout << std::setw(30) << "PagefileUsage: " << proc.PagefileUsage << " bytes" << std::endl;
    std::cout << std::setw(30) << "PeakPagefileUsage: " << proc.PeakPagefileUsage << " bytes" << std::endl;
#endif
}
