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
#include <iomanip>

const uint32_t N = 0x10;
struct Result {
    uint32_t Count;
    uint32_t Total;
} Table[0x1000];
void Recurse(NL::Node n) {
    if (!n.Num()) return;
    uint32_t last = (uint32_t)__rdtsc();
    for (NL::Node nn : n) {
        NL::String s = nn.Name();
        for (uint32_t i = N; i; --i) {
            if (n[s] != nn) throw;
        }
    }
    uint32_t now  = (uint32_t)__rdtsc();
    Table[n.Num()].Count++;
    Table[n.Num()].Total += now - last;
    for (NL::Node nn : n) Recurse(nn);
}

int main() {
    {
        NL::File file("Data.nx");
        Recurse(file.Base());
        for (uint32_t i = 0; i < 0x1000; ++i) {
            if (Table[i].Count) {
                std::cout << std::setw(4) << i << ": " << std::setw(3) << Table[i].Total / (Table[i].Count * i * N) << std::endl;
            }
        }
    }
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS proc;
    GetProcessMemoryInfo(GetCurrentProcess(), &proc, sizeof(proc));
    std::cout << std::setw(28) << "PageFaultCount: "             << std::setw(8) << proc.PageFaultCount             << " times" << std::endl;
    std::cout << std::setw(28) << "PeakWorkingSetSize: "         << std::setw(8) << proc.PeakWorkingSetSize         << " bytes" << std::endl;
    std::cout << std::setw(28) << "WorkingSetSize: "             << std::setw(8) << proc.WorkingSetSize             << " bytes" << std::endl;
    std::cout << std::setw(28) << "QuotaPeakPagedPoolUsage: "    << std::setw(8) << proc.QuotaPeakPagedPoolUsage    << " bytes" << std::endl;
    std::cout << std::setw(28) << "QuotaPagedPoolUsage: "        << std::setw(8) << proc.QuotaPagedPoolUsage        << " bytes" << std::endl;
    std::cout << std::setw(28) << "QuotaPeakNonPagedPoolUsage: " << std::setw(8) << proc.QuotaPeakNonPagedPoolUsage << " bytes" << std::endl;
    std::cout << std::setw(28) << "QuotaNonPagedPoolUsage: "     << std::setw(8) << proc.QuotaNonPagedPoolUsage     << " bytes" << std::endl;
    std::cout << std::setw(28) << "PeakPagefileUsage: "          << std::setw(8) << proc.PeakPagefileUsage          << " bytes" << std::endl;
    std::cout << std::setw(28) << "PagefileUsage: "              << std::setw(8) << proc.PagefileUsage              << " bytes" << std::endl;
#endif
}
