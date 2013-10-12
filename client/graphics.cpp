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

#include "graphics.hpp"
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <memory>
#include <string>
#include <chrono>
#include "config.hpp"
#include "time.hpp"
#include "game.hpp"

namespace nl {
    namespace graphics {
        std::unique_ptr<sf::Window> window {};
        std::string title {"NoLifeStory"};
        sf::ContextSettings const context {0, 0, 0, 1, 5};
        GLuint vbo {};
        void recreate_window(bool fullscreen) {
            config::fullscreen = fullscreen;
            if (config::fullscreen) window->create(sf::VideoMode(config::fullscreen_width, config::fullscreen_height, 32), title, sf::Style::Fullscreen, context);
            else window->create(sf::VideoMode(config::window_width, config::window_height, 32), title, sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close, context);
            //View::Resize(window->getSize().x, window->getSize().y);
            if (config::vsync) window->setVerticalSyncEnabled(true);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_TEXTURE_2D);
            glColor4f(1, 1, 1, 1);
            glClearColor(0, 0, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glVertexPointer(2, GL_FLOAT, 0, nullptr);
            glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
        }
        void init() {
            window = std::make_unique<sf::Window>();
            GLenum err {glewInit()};
            switch (err) {
            case GLEW_OK:
                break;
            case GLEW_ERROR_NO_GL_VERSION:
                throw std::runtime_error {"You need OpenGL"};
            case GLEW_ERROR_GL_VERSION_10_ONLY:
                throw std::runtime_error {"You need something newer than OpenGL 1.0"};
            case GLEW_ERROR_GLX_VERSION_11_ONLY:
                throw std::runtime_error {"You need something newer than GLX 1.1"};
            default:
                throw std::runtime_error {"ERROR: Unknown GLEW error code " + std::to_string(err)};
            }
            if (!GLEW_ARB_texture_non_power_of_two || !GLEW_VERSION_1_5) {
                throw std::runtime_error {"Your OpenGL is out of date. Please update your drivers and/or buy a new GPU"};
            }
            float a[] {0, 0, 1, 0, 1, 1, 0, 1};
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(a), a, GL_STATIC_DRAW);
            recreate_window(config::fullscreen);
        }
        void update() {
            if (!config::fullscreen) {
                //Too many title updates can cause bad performance
                static std::chrono::steady_clock::time_point lasttitle {std::chrono::steady_clock::now()};
                if (std::chrono::steady_clock::now() - lasttitle > std::chrono::milliseconds {250}) {
                    lasttitle = std::chrono::steady_clock::now();
                    window->setTitle("NoLifeStory {fps = " + std::to_string(time::fps) + ";};");
                }
            }
            window->display();
            GLenum err {glGetError()};
            switch (err) {
            case GL_NO_ERROR:
                break;
            case GL_INVALID_ENUM:
                throw std::runtime_error {"Invalid enum"};
            case GL_INVALID_VALUE:
                throw std::runtime_error {"Invalid value"};
            case GL_INVALID_OPERATION:
                throw std::runtime_error {"Invalid operation"};
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                throw std::runtime_error {"Invalid framebuffer operation"};
            case GL_OUT_OF_MEMORY:
                throw std::runtime_error {"Out of memory"};
            default:
                throw std::runtime_error {"Unknown OpenGL error code " + std::to_string(err)};
            }
            sf::Event e {};
            while (window->pollEvent(e)) switch (e.type) {
            case sf::Event::Closed:
                game::shut_down();
                break;
            case sf::Event::GainedFocus: break;
            case sf::Event::LostFocus: break;
            case sf::Event::JoystickButtonPressed: break;
            case sf::Event::JoystickButtonReleased: break;
            case sf::Event::JoystickConnected: break;
            case sf::Event::JoystickDisconnected: break;
            case sf::Event::JoystickMoved: break;
            case sf::Event::KeyPressed:
                switch (e.key.code) {
                case sf::Keyboard::M:
                    //BGM.setVolume(BGM.getVolume() > 0 ? 0 : 100);
                    break;
                case sf::Keyboard::R:
                    config::rave = !config::rave;
                    //BGM.PlayMusic();
                    break;
                case sf::Keyboard::F11:
                    //Create(!Config::Fullscreen);
                    break;
                case sf::Keyboard::Return:
                    //Map::Random();
                    break;
                case sf::Keyboard::Escape:
                    game::shut_down();
                    break;
                case sf::Keyboard::LAlt:
                    //Player::Pos.Jump();
                    break;
                case sf::Keyboard::Up:
                    //Player::Pos.up = true;
                    break;
                case sf::Keyboard::Down:
                    //Player::Pos.down = true;
                    break;
                case sf::Keyboard::Left:
                    //Player::Pos.left = true;
                    break;
                case sf::Keyboard::Right:
                    //Player::Pos.right = true;
                    break;
                default: break;
                }
                break;
            case sf::Event::KeyReleased: break;
            case sf::Event::MouseButtonPressed: break;
            case sf::Event::MouseButtonReleased: break;
            case sf::Event::MouseEntered: break;
            case sf::Event::MouseLeft: break;
            case sf::Event::MouseMoved: break;
            case sf::Event::MouseWheelMoved: break;
            case sf::Event::Resized:
                //View::Resize(e.size.width, e.size.height);
                break;
            case sf::Event::TextEntered: break;
            }
            glClear(GL_COLOR_BUFFER_BIT);
        }
        void unload() {
            window.reset();
        }
    }
}
