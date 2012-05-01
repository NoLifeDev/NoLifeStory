///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include "wzmain.h"

namespace WZ {
    path Path;
    Node WZ;
    vector<path> Paths;
    bool Lazy;
    class File {
    public:
        File(Node n);
    private:
        void Directory(Node n);
        uint32_t fileStart;
        MapFile file;
        bool big;
    };
    File::File(Node n) {
        big = false;
        path filename = Path/path(n.Name()+".wz");
        if (!exists(filename)) die();
        file.Open(filename);
        file.Map(0, 0x100);
        if (strcmp(file.ReadString(4), "PKG1") != 0) die();
        uint64_t fileSize = file.Read<uint64_t>();
        fileStart = file.Read<uint32_t>();
        file.ReadString();
        if (!Version) {
            EncVersion = file.Read<int16_t>();
            int32_t count = file.ReadCInt();
            uint32_t c = 0;
            for (int k = 0; k < count; ++k) {
                uint8_t type = file.Read<uint8_t>();
                if (type == 3) {
                    file.ReadEncString();
                    file.ReadCInt();
                    file.ReadCInt();
                    file.Read<uint32_t>();
                    continue;
                } else if (type == 4) {
                    file.ReadEncString();
                    file.ReadCInt();
                    file.ReadCInt();
                    c = file.Tell();
                    break;
                } else die();
            }
            if (c == 0) die();
            bool success = false;
            for (uint8_t j = 0; j < 3 && !success; ++j) {
                Key = Keys[j];
                AKey = AKeys[j];
                WKey = WKeys[j];
                for (Version = 0; Version < 512; ++Version) {
                    string s = to_string(Version);
                    size_t l = s.length();
                    VersionHash = 0;
                    for (int i = 0; i < l; ++i) VersionHash = 32*VersionHash+s[i]+1;
                    uint32_t result = 0xFF^(VersionHash>>24)^(VersionHash<<8>>24)^(VersionHash<<16>>24)^(VersionHash<<24>>24);
                    if (result == EncVersion) {
                        file.Map(0, 0x100);
                        file.Seek(c);
                        uint32_t off = file.ReadOffset(fileStart);
                        if (off > fileSize) continue;
                        file.Map(off, 0x100);
                        uint8_t a = file.Read<uint8_t>();
                        if(a != 0x73) continue;
                        string ss = file.ReadEncString();
                        if (ss != "Property") continue;
                        success = true;
                        break;
                    }
                }
            }
            if (!success) die();
            file.Map(0, 0x100);
            file.Seek(fileStart+2);
        } else {
            int16_t eversion = file.Read<int16_t>();
            if (eversion != EncVersion) die();
        }
        if (file.ReadCInt() == 0) die();
        file.Seek(fileStart+2);
        Directory(n);
        file.Unmap();
        delete this;
    }

    void File::Directory(Node n) {
        int32_t count = file.ReadCInt();
        if (count == 0) {
            new File(n);
            return;
        }
        vector<Node> dirs;
        n.Reserve(count);
        for (int i = 0; i < count; ++i) {
            char* name;
            uint8_t type = file.Read<uint8_t>();
            if (type == 1) {
                file.Skip(10);
                die();
                continue;
            } else if (type == 2) {
                int32_t s = file.Read<int32_t>();
                uint32_t p = file.Tell();
                file.Seek(fileStart+s);
                type = file.Read<uint8_t>();
                name = file.ReadEncString();
                file.Seek(p);
            } else if (type == 3) name = file.ReadEncString();
            else if (type == 4) name = file.ReadEncString();
            else die();
            uint32_t fsize = file.ReadCInt();
            uint32_t checksum = file.ReadCInt();
            uint32_t off = file.ReadOffset(fileStart);
            if (type == 3) {
                dirs.emplace_back(n.g(name, i));
            } else if (type == 4) {
                size_t len = strlen(name);
                name[len-4] = '\0';
                if (!big) {
                    big = true;
                    uint32_t p = file.Tell();
                    file.Map(0, off);
                    file.Seek(p);
                }
                new Img(file, n.g(name, i), fsize, off);
            } else die();
        }
        for (Node nn : dirs) {
            Directory(nn);
        }
    }

    void Load(string name) {
        WZ.InitTop(name);
        new File(WZ);
        if (!Lazy) {
            for (Img* img : Img::Imgs) img->Parse();
            WZ::WZ.Resolve();
        }
    }

    void Init(bool lazy) {
        GenKeys();
        Lazy = lazy;
        for (path p : Paths) {
            Path = p;
            if (exists(Path/path("Data.wz"))) {
                Load("Data");
                return;
            }
            if (exists(Path/path("Base.wz"))) {
                Load("Base");
                return;
            }
        }
        die();
    }

    void AddPath(string path) {
        Paths.push_back(path);
    }
}