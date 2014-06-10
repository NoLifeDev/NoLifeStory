//////////////////////////////////////////////////////////////////////////////
// NoLifeClient - Part of the NoLifeStory project                           //
// Copyright © 2014 Peter Atashian                                          //
//                                                                          //
// This program is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU Affero General Public License as           //
// published by the Free Software Foundation, either version 3 of the       //
// License, or (at your option) any later version.                          //
//                                                                          //
// This program is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
// GNU Affero General Public License for more details.                      //
//                                                                          //
// You should have received a copy of the GNU Affero General Public License //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//////////////////////////////////////////////////////////////////////////////

#include "game.hpp"
#include "log.hpp"

#include <array>
#include <csignal>
#include <fstream>
#include <iostream>
#include <memory>

#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
#endif

namespace nl {
void print_stack_trace() {
#ifdef _WIN32
    auto process = ::GetCurrentProcess();
    ::SymInitialize(process, nullptr, true);
    std::array<void *, 0x100> stack;
    auto frames
        = ::CaptureStackBackTrace(0, static_cast<::DWORD>(stack.size()), stack.data(), nullptr);
    std::array<char, sizeof(::SYMBOL_INFO) + 0x100> symbol_raw;
    auto symbol = reinterpret_cast<::SYMBOL_INFO *>(symbol_raw.data());
    symbol->MaxNameLen = 0xFF;
    symbol->SizeOfStruct = sizeof(::SYMBOL_INFO);
    log << "Printing stack trace" << std::endl;
    IMAGEHLP_LINE64 line;
    line.SizeOfStruct = sizeof(line);
    DWORD disp;
    for (auto i = decltype(frames){0}; i < frames; ++i) {
        auto s = static_cast<::DWORD64>(reinterpret_cast<uintptr_t>(stack[i]));
        ::SymFromAddr(process, s, 0, symbol);
        ::SymGetLineFromAddr64(process, s, &disp, &line);
        log << std::hex << symbol->Address << ": " << symbol->Name << " at " << std::dec
            << line.FileName << ":" << line.LineNumber << std::endl;
    }
    log << "End of stack trace" << std::endl;
#endif
}
void terminate_handler() {
    log << "Unhandled exception!" << std::endl;
    print_stack_trace();
    std::abort();
}

void sigsegv_handler(int) {
    log << "Segfault!" << std::endl;
    print_stack_trace();
    std::abort();
}
void client() {
    log.open("NoLifeClient.log", std::ios::app);
    if (!log.is_open()) {
        std::cerr << "Failed to open log" << std::endl;
        std::abort();
    }
    std::set_terminate(terminate_handler);
    std::signal(SIGSEGV, sigsegv_handler);
    log << "Starting up NoLifeClient" << std::endl;
    try {
        game::play();
    } catch (std::exception &e) { log << "Fatal exception: " << e.what() << std::endl; }
    log << "Shutting down NoLifeClient" << std::endl;
}
}

int main() { nl::client(); }

#ifdef _WIN32
int __stdcall WinMain(HINSTANCE, HINSTANCE, char *, int) {
    nl::client();
    return 0;
}
#endif
