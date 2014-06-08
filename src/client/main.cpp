//////////////////////////////////////////////////////////////////////////////
// NoLifeClient - Part of the NoLifeStory project                           //
// Copyright © 2013 Peter Atashian                                          //
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
#include <iostream>
#include <memory>
#include <fstream>

void terminated() {
    std::cerr << "Fatal termination!" << std::endl;
    std::abort();
}

int main() {
    try {
        nl::log.open("NoLifeClient.log", std::ios::app);
        std::set_terminate(terminated);
        nl::log << "Starting up NoLifeClient" << std::endl;
        nl::game::play();
        nl::log << "Shutting down NoLifeClient" << std::endl;
        return EXIT_SUCCESS;
    } catch (std::exception const & e) {
        nl::log << "Fatal uncaught exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        nl::log << "Fatal unknown exception!" << std::endl;
        return EXIT_FAILURE;
    }
}
#ifdef _MSC_VER
int __stdcall wWinMain(void *, void *, wchar_t *, int) {
    main();
}
#endif
