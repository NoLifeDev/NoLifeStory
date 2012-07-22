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
#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#include <thread>
using namespace std;
using namespace NL;

int main() {
    LARGE_INTEGER freq, last, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last);
    Load("Data.nx");
    QueryPerformanceCounter(&now);
    cout << "Time taken: " << 1000.*(now.QuadPart-last.QuadPart)/freq.QuadPart << "ms" << endl;
    PROCESS_MEMORY_COUNTERS proc;
    GetProcessMemoryInfo(GetCurrentProcess(), &proc, sizeof(proc));
    cout << "Memory usage: " << proc.PeakPagefileUsage/1000 << "KB" << endl;
    
}