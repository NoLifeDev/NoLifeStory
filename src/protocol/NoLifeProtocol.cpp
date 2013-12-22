//////////////////////////////////////////////////////////////////////////////
// NoLifeProtocol - Part of the NoLifeStory project                         //
// Copyright (C) 2013 Peter Atashian                                        //
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
#include "NoLifeProtocol.hpp"
#include <openssl/rsa.h>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <iostream>
namespace NL {
    SecureSocket::SecureSocket() {
        sf::TcpSocket socket;
        bool connected = false;
        while (!connected) {
            switch (socket.connect("localhost", 273, sf::seconds(5))) {
            case sf::Socket::Status::Disconnected:
                std::cout << "Disconnected" << std::endl;
                break;
            case sf::Socket::Status::Done:
                std::cout << "Connected" << std::endl;
                connected = true;
                break;
            case sf::Socket::Status::Error:
                std::cout << "Error" << std::endl;
                break;
            case sf::Socket::Status::NotReady:
                std::cout << "Not Ready" << std::endl;
                break;
            }
        }
    }
    SecureListener::SecureListener() {
        sf::TcpListener listen;
        sf::TcpSocket socket;
        bool connected = false;
        while (!connected) {
            switch (listen.accept(socket)) {
                case sf::Socket::Status::Disconnected:
                std::cout << "Disconnected" << std::endl;
                break;
            case sf::Socket::Status::Done:
                std::cout << "Connected" << std::endl;
                connected = true;
                break;
            case sf::Socket::Status::Error:
                std::cout << "Error" << std::endl;
                break;
            case sf::Socket::Status::NotReady:
                std::cout << "Not Ready" << std::endl;
                break;
            }
        }
    }
}