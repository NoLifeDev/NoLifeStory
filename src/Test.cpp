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
#include "NX.hpp"
#include <iostream>
#include <ctime>

//Uncomment the following to observe the sprites. Will be much slower however.
//#define GFX
#ifdef GFX
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
sf::Window * win;
const size_t width = 800, height = 600, minwidth = 200, minheight = 150;
#endif

const NL::File file("Data.nx");
size_t c;

void Recurse(NL::Node n) {
    if (n.T() == NL::Node::Type::bitmap) {
        NL::Bitmap b = n;
#ifdef GFX
        if (b.w < minwidth && b.h < minheight) return;
        sf::Event e;
        while (win->pollEvent(e));
        glClear(GL_COLOR_BUFFER_BIT);
        GLuint t;
        glGenTextures(1, &t);
        glBindTexture(GL_TEXTURE_2D, t);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, b.Width(), b.Height(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, b.Get());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glBegin(GL_QUADS);
        glTexCoord2i(0, 0);
        glVertex2i(0, 0);
        glTexCoord2i(1, 0);
        glVertex2i(b.Width(), 0);
        glTexCoord2i(1, 1);
        glVertex2i(b.Width(), b.Height());
        glTexCoord2i(0, 1);
        glVertex2i(0, b.Height());
        glEnd();
        glDeleteTextures(1, &t);
        win->display();
#else
        b.Get();
#endif
        c++;
    }
    for (auto nn : n) Recurse(nn);
}

template <typename T>
void test(T f) {
    for (;;) {
        c = 0;
        size_t t1 = clock();
        f(file.Base());
        size_t t2 = clock();
        std::cout << c << " bitmaps in " << (t2 - t1) * 1000 / CLOCKS_PER_SEC << " ms" << std::endl;
    }
}

int main() {
#ifdef GFX
    win = new sf::Window(sf::VideoMode(width, height, 32), "NoLifeNx", sf::Style::Close);
    gluOrtho2D(0, width, height, 0);
    glColor4f(1, 1, 1, 1);
    glClearColor(0, 0, 0, 0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
    test(Recurse);
}