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

#include "sound.hpp"
#include "map.hpp"
#include "config.hpp"
#include <nx/audio.hpp>
#include <nx/nx.hpp>
#include <mpg123.h>
#include <RtAudio.h>
#include <memory>
#include <locale>

namespace nl {
    namespace music {
        mpg123_handle * handle = nullptr;
        node n = {};
        std::unique_ptr<RtAudio> dac;
        unsigned int frames;
        int channels;
        void init() {
            if (mpg123_init() != MPG123_OK)
                throw std::runtime_error("Failed to initialize mpg123");
            dac = std::make_unique<RtAudio>();
            if (!dac->getDeviceCount())
                throw std::runtime_error("No audio device found!");
        }
        int callback(void * output, void *, unsigned int frames, double, RtAudioStreamStatus, void *) {
            size_t todo = frames * 2 * channels;
            auto buf = reinterpret_cast<unsigned char *>(output);
            while (todo) {
                size_t done;
                auto err = mpg123_read(handle, buf, todo, &done);
                todo -= done;
                buf += done;
                if (err == MPG123_NEED_MORE) {
                    audio a = n;
                    mpg123_feed(handle, reinterpret_cast<unsigned char const *>(a.data()) + 82, a.length() - 82);
                }
            }
            return 0;
        }
        void play(node nn) {
            if (n == nn)
                return;
            n = nn;
            audio a = n;
            if (handle)
                mpg123_close(handle);
            if (dac->isStreamOpen())
                dac->closeStream();
            if (!a) {
                std::cerr << "Map does not contain valid bgm" << std::endl;
                return;
            }
            handle = mpg123_new(nullptr, nullptr);
            if (!handle)
                throw std::runtime_error("Failed to open mpg123 handle");
            mpg123_open_feed(handle);
            mpg123_feed(handle, reinterpret_cast<unsigned char const *>(a.data()) + 82, a.length() - 82);
            long rate;
            int encoding;
            if (mpg123_getformat(handle, &rate, &channels, &encoding) != MPG123_OK)
                throw std::runtime_error("Failed to get format of music");
            RtAudio::StreamParameters parameters;
            parameters.deviceId = dac->getDefaultOutputDevice();
            parameters.nChannels = channels;
            parameters.firstChannel = 0;
            frames = mpg123_outblock(handle);
            dac->openStream(&parameters, nullptr, RTAUDIO_SINT16, rate, &frames, callback);
            dac->startStream();
        }
        void play(std::string s) {
            if (!n && dac->isStreamRunning())
                return;
            n = {};
        }
        void play_bgm() {
            std::string bgm = map::current["info"]["bgm"];
            if (islower(bgm[0]))
                bgm[0] = std::toupper(bgm[0], std::locale::classic());
            while (bgm.find(' ') != bgm.npos)
                bgm.erase(bgm.find(' '), 1);
            auto p = bgm.find('/');
            auto sn = nx::sound[bgm.substr(0, p) + ".img"][bgm.substr(p + 1)];
            if (!sn)
                std::cerr << "Failed to find bgm " << bgm << " for map " << map::current_name << std::endl;
            play(sn);
        }
    }
    /*
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
    }*/
}