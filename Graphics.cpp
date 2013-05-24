//////////////////////////////////////////////////////////////////////////////
// NoLifeClient - Part of the NoLifeStory project                           //
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
#include "NoLifeClient.hpp"
namespace NL {
    namespace Graphics {
        sf::Window * Window;
        uint32_t Width, Height;
        uint32_t WWidth = 800, WHeight = 600;
        uint32_t FWidth, FHeight;
        string Title = "InfinityCubed";
        bool Fullscreen = false;
        const sf::ContextSettings Context(0, 0, 0, 1, 1);
        void AdjustView(uint32_t w, uint32_t h) {
            Width = w;
            Height = h;
            glViewport(0, 0, Width, Height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, WWidth, WHeight, 0, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
        void Create(bool fullscreen) {
            if (fullscreen) Window->create(sf::VideoMode(FWidth, FHeight, 32), Title, sf::Style::Default | sf::Style::Fullscreen, Context);
            else Window->create(sf::VideoMode(WWidth, WHeight, 32), Title, sf::Style::Default, Context);
            AdjustView(Window->getSize().x, Window->getSize().y);
            Fullscreen = fullscreen;
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_TEXTURE_2D);
            glColor4f(1, 1, 1, 1);
            glClearColor(0, 0, 0, 0);
        }
        void Init() {
            auto v = sf::VideoMode::getFullscreenModes()[0];
            FWidth = v.width;
            FHeight = v.height;
            Window = new sf::Window();
            Create(false);
            BASS_Init(-1, 44100, 0, Window->getSystemHandle(), 0);
            GLenum err = glewInit();
            switch (err) {
            case GLEW_OK:
                break;
            case GLEW_ERROR_NO_GL_VERSION:
                Log::Write("ERROR: You need OpenGL");
                throw; break;
            case GLEW_ERROR_GL_VERSION_10_ONLY:
                Log::Write("ERROR: You need something newer than OpenGL 1.0");
                throw; break;
            case GLEW_ERROR_GLX_VERSION_11_ONLY:
                Log::Write("ERROR: You need something newer than GLX 1.1");
                throw; break;
            default:
                Log::Write("ERROR: Unknown GLEW error code " + to_string(err));
                throw;
            }
            if (!GLEW_ARB_texture_non_power_of_two) {
                Log::Write("ERROR: Your OpenGL lacks needed extensions");
                Log::Write("Please update your drivers and/or buy a new GPU");
                throw;
            }
        }
        void Update() {
            Window->setTitle(Title + " at " + to_string(Time::FPS) + " FPS");
            Window->display();
            GLenum err = glGetError();
            switch (err) {
            case GL_NO_ERROR:
                break;
            case GL_INVALID_ENUM:
                Log::Write("ERROR: Invalid enum");
                throw; break;
            case GL_INVALID_VALUE:
                Log::Write("ERROR: Invalid value");
                throw; break;
            case GL_INVALID_OPERATION:
                Log::Write("ERROR: Invalid operation");
                throw; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                Log::Write("ERROR: Invalid framebuffer operation");
                throw; break;
            case GL_OUT_OF_MEMORY:
                Log::Write("ERROR: Out of memory");
                throw; break;
            default:
                Log::Write("ERROR: Unknown OpenGL error code " + to_string(err));
                throw;
            }
            sf::Event e;
            while (Window->pollEvent(e)) switch (e.type) {
            case sf::Event::Closed:
                Game::Over = true;
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
                case sf::Keyboard::F11:
                    Create(!Fullscreen);
                    break;
                case sf::Keyboard::Escape:
                    Game::Over = true;
                    break;
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
                AdjustView(e.size.width, e.size.height);
                break;
            case sf::Event::TextEntered: break;
            }
            glClear(GL_COLOR_BUFFER_BIT);
        }
    }
}