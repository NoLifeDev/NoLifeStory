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

#include "NoLifeClient.hpp"
#include <mpg123.h>

namespace NL {
    Music BGM;
    Music::Music() {
        if (mpg123_init() != MPG123_OK) throw "Failed to initialize mpg123";
        handle = 0;
        node = Node();
    }
    void Music::LoadNode(Node n) {
        if (!n) return;
        if (n == node) return;
        stop();
        node = n;
        raw = false;
        Audio a = node;
        if (handle) mpg123_close(reinterpret_cast<mpg123_handle *>(handle));
        handle = mpg123_new(nullptr, nullptr);
        if (!handle) throw "Failed to create mpg123 handle";
        mpg123_open_feed(reinterpret_cast<mpg123_handle *>(handle));
        mpg123_feed(reinterpret_cast<mpg123_handle *>(handle), reinterpret_cast<unsigned char const *>(a.Data()), a.Length());
        long rate(0);
        int channels(0), encoding(0);
        if (mpg123_getformat(reinterpret_cast<mpg123_handle *>(handle), &rate, &channels, &encoding) == MPG123_OK) {
            buf.resize(mpg123_outblock(reinterpret_cast<mpg123_handle *>(handle)));
            initialize(channels, rate);
        } else {
            buf.clear();
            buf.insert(buf.begin(), reinterpret_cast<char const *>(a.Data()) , reinterpret_cast<char const *>(a.Data()) + a.Length());
            initialize(1, 22050);
            raw = true;
        }
        
    }
    void Music::LoadFile(string s) {
        stop();
        node = Node();
        raw = false;
        if (handle) mpg123_close(reinterpret_cast<mpg123_handle *>(handle));
        handle = mpg123_new(nullptr, nullptr);
        if (!handle) throw "Failed to create mpg123 handle";
        mpg123_open(reinterpret_cast<mpg123_handle *>(handle), s.c_str());
        long rate(0);
        int channels(0), encoding(0);
        if (mpg123_getformat(reinterpret_cast<mpg123_handle *>(handle), &rate, &channels, &encoding) != MPG123_OK) throw "Failed to obtain mpg123 handle format";
        buf.resize(mpg123_outblock(reinterpret_cast<mpg123_handle *>(handle)));
        initialize(channels, rate);
    }
    bool Music::onGetData(sf::SoundStream::Chunk & data) {
        if (raw) {
            data.samples = reinterpret_cast<sf::Int16 const *>(buf.data());
            data.sampleCount = buf.size() / sizeof(sf::Int16);
            return false;
        } else {
            size_t done;
            mpg123_read(reinterpret_cast<mpg123_handle *>(handle), buf.data(), buf.size(), &done);
            data.samples = reinterpret_cast<sf::Int16 const *>(buf.data());
            data.sampleCount = done / sizeof(sf::Int16);
            return data.sampleCount > 0;
        }
    }
    void Music::onSeek(sf::Time t) {
        if (raw) {
        } else if (node) {
            Audio a = node;
            off_t o;
            mpg123_feedseek(reinterpret_cast<mpg123_handle *>(handle), t.asSeconds(), SEEK_SET, &o);
            mpg123_feed(reinterpret_cast<mpg123_handle *>(handle), reinterpret_cast<unsigned char const *>(a.Data()) + o, a.Length() - o);
        } else mpg123_seek(reinterpret_cast<mpg123_handle *>(handle), t.asSeconds(), SEEK_SET);
    }
    void Music::PlayMusic() {
        if (Config::Rave) {
            if (node || getStatus() == Stopped) {
                LoadFile("bgm.mp3");
                setLoop(true);
                play();
            }
        } else {
            string bgm(Map::Current["info"]["bgm"]);
            if (islower(bgm[0])) bgm[0] = toupper(bgm[0]);
            while (bgm.find(' ') != bgm.npos) bgm.erase(bgm.find(' '), 1);
            size_t p(bgm.find('/'));
            Node sn(NXSound[bgm.substr(0, p) + ".img"][bgm.substr(p + 1)]);
            if (!sn) Log::Write("Failed to find bgm " + bgm + " for map " + Map::Current.Name());
            if (sn == node) return;
            LoadNode(sn);
            setLoop(true);
            play();
        }
    }
}