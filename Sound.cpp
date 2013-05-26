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
    Sound::Sound() : d(), s(0) {}
    Sound::Sound(Sound const & o) : d(o.d), s(0) {}
    Sound::Sound(Sound && o) : d(), s(0) {
        swap(d, o.d);
        swap(s, o.s);
    }
    Sound::Sound(Node n) : d(n), s(0) {}
    Sound::~Sound() {
        if (s) {
            BASS_ChannelStop(s);
            BASS_StreamFree(s);
        }
    }
    Sound & Sound::operator=(Sound const & o) {
        if (d == o.d) return *this;
        d = o.d;
        s = 0;
        return *this;
    }
    Sound & Sound::operator=(Sound && o) {
        if (d == o.d) return *this;
        swap(d, o.d);
        swap(s, o.s);
        return *this;
    }
    void Sound::Play(bool loop) {
        if (!d) return;
        if (!s) {
            if (loop) s = BASS_StreamCreateFile(true, d.Data(), 0, d.Length(), BASS_SAMPLE_FLOAT | BASS_SAMPLE_LOOP);
            else s = BASS_StreamCreateFile(true, d.Data(), 0, d.Length(), BASS_SAMPLE_FLOAT);
        }
        BASS_ChannelPlay(s, !loop);
    }
    void Sound::Stop() {
        BASS_ChannelStop(s);
    }
    void Sound::SetVolume(float v) {
        BASS_ChannelSetAttribute(s, BASS_ATTRIB_VOL, v);
    }
    float Sound::GetVolume() {
        float v;
        BASS_ChannelGetAttribute(s, BASS_ATTRIB_VOL, &v);
        return v;
    }
}