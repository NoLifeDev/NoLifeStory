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
#include <iostream>

void terminated() {
    std::cerr << "Fatal termination!" << std::endl;
    std::abort();
}

int main(int, char **) {
    try {
        std::set_terminate(terminated);
        std::freopen("NoLifeClient.log", "a", stderr);
        std::freopen("NoLifeClient.log", "a", stdout);
        std::cout << "Starting up NoLifeClient" << std::endl;
        nl::game::play();
        std::cout << "Shutting down NoLifeClient" << std::endl;
        return EXIT_SUCCESS;
    } catch (std::exception const & e) {
        std::cerr << "Fatal uncaught exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Fatal unknown exception!" << std::endl;
        return EXIT_FAILURE;
    }
}
