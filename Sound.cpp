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
        mpg123_feed(handle, (unsigned char *)a.Data(), a.Length());
        long rate(0);
        int channels(0), encoding(0);
        if (mpg123_getformat(handle, &rate, &channels, &encoding) != MPG123_OK) throw;
        buf.resize(mpg123_outblock(handle));
        initialize(channels, rate);
    }
    void Music::LoadFile(string s) {
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
        data.samples = (sf::Int16 *)buf.data();
        data.sampleCount = done / sizeof(sf::Int16);
        return data.sampleCount > 0;
    }
    void Music::onSeek(sf::Time t) {
        mpg123_seek(handle, t.asSeconds(), SEEK_SET);
    }
    //Sound::Sound() : d(), s(0) {}
    //Sound::Sound(Sound const & o) : d(o.d), s(0) {}
    //Sound::Sound(Sound && o) : d(), s(0) {
    //    swap(d, o.d);
    //    swap(s, o.s);
    //}
    //Sound::Sound(Node n) : d(n), s(0) {}
    //Sound::~Sound() {
    //    if (s) {
    //        BASS_ChannelStop(s);
    //        BASS_StreamFree(s);
    //    }
    //}
    //Sound & Sound::operator=(Sound const & o) {
    //    if (d == o.d) return *this;
    //    d = o.d;
    //    s = 0;
    //    return *this;
    //}
    //Sound & Sound::operator=(Sound && o) {
    //    if (d == o.d) return *this;
    //    swap(d, o.d);
    //    swap(s, o.s);
    //    return *this;
    //}
    //void Sound::Play(bool loop) {
    //    if (!d) return;
    //    if (!s) {
    //        if (loop) s = BASS_StreamCreateFile(true, d.Data(), 0, d.Length(), BASS_SAMPLE_FLOAT | BASS_SAMPLE_LOOP);
    //        else s = BASS_StreamCreateFile(true, d.Data(), 0, d.Length(), BASS_SAMPLE_FLOAT);
    //    }
    //    BASS_ChannelPlay(s, !loop);
    //}
    //void Sound::Stop() {
    //    BASS_ChannelStop(s);
    //}
    //void Sound::SetVolume(float v) {
    //    BASS_ChannelSetAttribute(s, BASS_ATTRIB_VOL, v);
    //}
    //float Sound::GetVolume() {
    //    float v;
    //    BASS_ChannelGetAttribute(s, BASS_ATTRIB_VOL, &v);
    //    return v;
    //}
}