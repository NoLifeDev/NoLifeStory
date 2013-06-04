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
    Music BGM;
    Music::Music() {
        if (mpg123_init() != MPG123_OK) throw;
        handle = 0;
    }
    void Music::LoadNode(Node n) {
        if (n == node) return;
        node = n;
        stop();
        Audio a = node;
        if (handle) mpg123_close(handle);
        handle = mpg123_new(nullptr, nullptr);
        if (!handle) throw;
        mpg123_open_feed(handle);
        mpg123_feed(handle, reinterpret_cast<unsigned char const *>(a.Data()), a.Length());
        long rate(0);
        int channels(0), encoding(0);
        if (mpg123_getformat(handle, &rate, &channels, &encoding) != MPG123_OK) throw;
        buf.resize(mpg123_outblock(handle));
        initialize(channels, rate);
    }
    void Music::LoadFile(string s) {
        node = Node();
        stop();
        if (handle) mpg123_close(handle);
        handle = mpg123_new(nullptr, nullptr);
        if (!handle) throw;
        mpg123_open(handle, s.c_str());
        long rate(0);
        int channels(0), encoding(0);
        if (mpg123_getformat(handle, &rate, &channels, &encoding) != MPG123_OK) throw;
        buf.resize(mpg123_outblock(handle));
        initialize(channels, rate);
    }
    bool Music::onGetData(sf::SoundStream::Chunk & data) {
        size_t done;
        mpg123_read(handle, buf.data(), buf.size(), &done);
        data.samples = reinterpret_cast<sf::Int16 const *>(buf.data());
        data.sampleCount = done / sizeof(sf::Int16);
        return data.sampleCount > 0;
    }
    void Music::onSeek(sf::Time t) {
        if (node) {
            Audio a = node;
            off_t o;
            mpg123_feedseek(handle, t.asSeconds(), SEEK_SET, &o);
            mpg123_feed(handle, reinterpret_cast<unsigned char const *>(a.Data()) + o, a.Length() - o);
        } else mpg123_seek(handle, t.asSeconds(), SEEK_SET);
    }
}