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

struct Result {
    size_t Count;
    size_t Total;
} Table[0x800];
void Recurse(NL::Node n) {
    if (!n.Num()) return;
    uint64_t last = __rdtsc();
    NL::Node nnn;
    for (NL::Node nn : n) nnn = n[nn.Name()];
    volatile int32_t x = nnn.X();
    uint64_t now = __rdtsc();
    Table[n.Num()].Count++;
    Table[n.Num()].Total += now - last;
    for (NL::Node nn : n) Recurse(nn);
}

int main() {
    NL::Node n;
    NL::File file("Data.nx");
    Recurse(file.Base());
    for (size_t i = 0; i < 0x800; ++i) if (Table[i].Count) std::cout << std::setw(4) << i << ": " << Table[i].Total / (Table[i].Count * i) << std::endl;
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS proc;
    GetProcessMemoryInfo(GetCurrentProcess(), &proc, sizeof(proc));
    std::cout << std::setw(28) << "PageFaultCount: " << proc.PageFaultCount << " times" << std::endl;
    std::cout << std::setw(28) << "PeakWorkingSetSize: " << proc.PeakWorkingSetSize << " bytes" << std::endl;
    std::cout << std::setw(28) << "WorkingSetSize: " << proc.WorkingSetSize << " bytes" << std::endl;
    std::cout << std::setw(28) << "QuotaPeakPagedPoolUsage: " << proc.QuotaPeakPagedPoolUsage << " bytes" << std::endl;
    std::cout << std::setw(28) << "QuotaPagedPoolUsage: " << proc.QuotaPagedPoolUsage << " bytes" << std::endl;
    std::cout << std::setw(28) << "QuotaPeakNonPagedPoolUsage: " << proc.QuotaPeakNonPagedPoolUsage << " bytes" << std::endl;
    std::cout << std::setw(28) << "QuotaNonPagedPoolUsage: " << proc.QuotaNonPagedPoolUsage << " bytes" << std::endl;
    std::cout << std::setw(28) << "PagefileUsage: " << proc.PagefileUsage << " bytes" << std::endl;
    std::cout << std::setw(28) << "PeakPagefileUsage: " << proc.PeakPagefileUsage << " bytes" << std::endl;
#endif
}
