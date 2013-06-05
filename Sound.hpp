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
#pragma once
namespace NL {
    class Music : public sf::SoundStream {
    public:
        Music();
        void LoadNode(Node);
        void LoadFile(string);
        void PlayMusic();
    protected:
        bool onGetData(sf::SoundStream::Chunk &);
        void onSeek(sf::Time);
    private:
        mpg123_handle * handle;
        Node node;
        vector<unsigned char> buf;
    };
    extern Music BGM;
}