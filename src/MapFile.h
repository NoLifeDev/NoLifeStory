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

namespace NL {
    class MapFile {
    public:
        void Open(string filename);
        uint64_t Tell();
        void* TellPtr();
        void Seek(uint64_t);
        void Skip(uint64_t);
        template <class T> T Read() {
            return *reinterpret_cast<T*>(ReadBin(sizeof(T)));
        }
        void* ReadBin(uint64_t size);
        string ReadString(uint16_t length);
        char* base;
        char* off;
    };
}