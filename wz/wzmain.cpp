///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include "wz.h"
#include "aes.h"
#include "wzmain.h"
#include "wzimg.h"

namespace WZ {
    path Path;
    Node WZ;
    vector<path> Paths;
    uint8_t *Key = 0;
    int16_t EncVersion;
    uint16_t Version = 0;
    uint32_t VersionHash;
    uint8_t Keys[3][0x10000];
    const uint8_t GMSKeyIV[4] = {
	    0x4D, 0x23, 0xC7, 0x2B
    };
    const uint8_t KMSKeyIV[4] = {
	    0xB9, 0x7D, 0x63, 0xE9
    };
    const uint8_t AESKey[] = {
	    0x13, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,	0x06, 0x00, 0x00, 0x00, 0xB4, 0x00, 0x00, 0x00,
	    0x1B, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,	0x33, 0x00, 0x00, 0x00, 0x52, 0x00, 0x00, 0x00
    };
    const uint32_t OffsetKey = 0x581C3F6D;
    AES AESGen;
    void GenKey(const uint8_t* IV, uint8_t* key) {
	    uint8_t BigIV[16];
	    for (int i = 0; i < 16; i += 4) {
		    memcpy(BigIV+i, IV, 4);
	    }
	    AESGen.SetParameters(256, 128);
	    AESGen.StartEncryption(AESKey);
	    AESGen.EncryptBlock(BigIV, key);
	    for (int i = 16; i < 0x10000; i += 16) {
		    AESGen.EncryptBlock(key+i-16, key+i);
	    }
    }
    void GenKeys() {
        memset(Keys[0], 0, 0x10000);
	    GenKey(GMSKeyIV, Keys[1]);
	    GenKey(KMSKeyIV, Keys[2]);
    }

#pragma region File reading stuff
    template <class T>
    T Read(FILE* file) {
        T v;
        fread(&v, sizeof(T), 1, file);
        return v;
    }
    int32_t ReadCInt(FILE* file) {
        int8_t a = Read<int8_t>(file);
        if (a != -128) return a;
        else {
            int32_t b = Read<int32_t>(file);
            return b;
        }
    }

    string ReadEncString(FILE* file) {
        int8_t slen = Read<int8_t>(file);
        if (slen == 0) return string();
        else if (slen > 0) {
            int32_t len;
            if (slen == 127) len = Read<int32_t>(file);
            else len = slen;
            if (len <= 0) return string();
            uint16_t mask = 0xAAAA;
            wstring ws(len, '0');
            fread(&ws[0], 2, len, file);
            for (int i = 0; i < len; ++i) {
                ws[i] ^= mask;
                ws[i] ^= *(uint16_t*)(Key+2*i);
                mask++;
            }
            wstring_convert<codecvt_utf8<wchar_t>> conv;
            return conv.to_bytes(ws);
        } else {
            int32_t len;
            if (slen == -128) len = Read<int32_t>(file);
            else len = -slen;
            if (len <= 0) return string();
            uint8_t mask = 0xAA;
            string s(len, '0');
            fread(&s[0], 1, len, file);
            for (int i = 0; i < len; ++i) {
                s[i] ^= mask;
                s[i] ^= Key[i];
                mask++;
            }
            return s;
        }
    }

    void ReadEncFast(FILE* file) {
        int8_t slen = Read<int8_t>(file);
        if (slen == 0) return;
        if (slen > 0) {
            int32_t len;
            if (slen == 127) len = Read<int32_t>(file);
            else len = slen;
            if (len <= 0) return;
            fseek(file, len*2, SEEK_CUR);
        } else {
            int32_t len;
            if (slen == -128) len = Read<int32_t>(file);
            else len = -slen;
            if (len <= 0) return;
            fseek(file, len, SEEK_CUR);
        }
    }

    string ReadString(FILE* file, uint32_t offset) {
        uint8_t a = Read<uint8_t>(file);
        switch (a) {
        case 0x00:
        case 0x73:
            return ReadEncString(file);
        case 0x01:
        case 0x1B:
            {
                offset += Read<int32_t>(file);
                uint32_t p = ftell(file);
                fseek(file, offset, SEEK_SET);
                string s = ReadEncString(file);
                fseek(file, p, SEEK_SET);
                return s;
            }
        default:
            return string();
        }
    }

    string ReadStringOffset(FILE* file, uint32_t offset) {
        uint32_t p = ftell(file);
        fseek(file, offset, SEEK_SET);
        string s = ReadEncString(file);
        fseek(file, p, SEEK_SET);
        return s;
    }
    uint32_t ReadOffset(FILE* file, uint32_t fileStart) {
        uint32_t p = ftell(file);
        p = (p-fileStart)^0xFFFFFFFF;
        p *= VersionHash;
        p -= OffsetKey;
        p = (p<<(p&0x1F))|(p>>(32-p&0x1F));
        uint32_t more = Read<uint32_t>(file);
        p ^= more;
        p += fileStart*2;
        return p;
    }

    void Directory(Node n, FILE* file, uint32_t fileStart);

    void File(Node n) {
        path filename = Path/path(n.Name()+".wz");
        if (!exists(filename)) die();
        FILE* file = fopen(string(filename).c_str(), "rb");
        if (!file) die();
        char ident[4];
        fread(ident, 1, 4, file);
        if (string(ident, 4) != "PKG1") die();
        uint64_t fileSize = Read<uint64_t>(file);
        uint32_t fileStart = Read<uint32_t>(file);
        string copyright;
        while (true) {
            char c = fgetc(file);
            if (c == '\0') break;
            copyright += c;
        }
        if (!Version) {
            EncVersion = Read<int16_t>(file);
            int32_t count = ReadCInt(file);
            uint32_t c = 0;
            for (int k = 0; k < count; ++k) {
                uint8_t type = Read<uint8_t>(file);
                if (type == 3) {
                    ReadEncFast(file);
                    ReadCInt(file);
                    ReadCInt(file);
                    Read<uint32_t>(file);
                    continue;
                } else if (type == 4) {
                    ReadEncFast(file);
                    ReadCInt(file);
                    ReadCInt(file);
                    c = ftell(file);
                    break;
                } else die();
            }
            if (c == 0) die();
            bool success = false;
            for (uint8_t j = 0; j < 3 && !success; ++j) {
                Key = Keys[j];
                for (Version = 0; Version < 512; ++Version) {
                    string s = to_string(Version);
                    int l = s.length();
                    VersionHash = 0;
                    for (int i = 0; i < l; ++i) VersionHash = 32*VersionHash+s[i]+1;
                    uint32_t result = 0xFF^(VersionHash>>24)^(VersionHash<<8>>24)^(VersionHash<<16>>24)^(VersionHash<<24>>24);
                    if (result == EncVersion) {
                        clearerr(file);
                        fseek(file, c, SEEK_SET);
                        uint32_t offset = ReadOffset(file, fileStart);
                        if (offset > fileSize) continue;
                        fseek(file, offset, SEEK_SET);
                        uint8_t a = Read<uint8_t>(file);
                        if(a != 0x73) continue;
                        string ss = ReadEncString(file);
                        if (ss != "Property") continue;
                        success = true;
                        break;
                    }
                }
            }
            if (!success) die();
            fseek(file, fileStart+2, SEEK_SET);
        } else {
            int16_t eversion = Read<int16_t>(file);
            if (eversion != EncVersion) die();
        }
        if (ReadCInt(file) == 0) die();
        fseek(file, fileStart+2, SEEK_SET);
        Directory(n, file, fileStart);
    }

    void Directory(Node n, FILE* file, uint32_t fileStart) {
        int32_t count = ReadCInt(file);
        if (count == 0) {
            File(n);
            return;
        }
        vector<string> dirs;
        for (int i = 0; i < count; ++i) {
            string name;
            uint8_t type = Read<uint8_t>(file);
            if (type == 1) {
                fseek(file, 10, SEEK_CUR);
                die();
                continue;
            } else if (type == 2) {
                int32_t s = Read<int32_t>(file);
                uint32_t p = ftell(file);
                fseek(file, fileStart+s, SEEK_SET);
                type = Read<uint8_t>(file);
                name = ReadEncString(file);
                fseek(file, p, SEEK_SET);
            } else if (type == 3) name = ReadEncString(file);
            else if (type == 4) name = ReadEncString(file);
            else die();
            uint32_t fsize = ReadCInt(file);
            uint32_t checksum = ReadCInt(file);
            uint32_t offset = ReadOffset(file, fileStart);
            if (type == 3) {
                dirs.emplace_back(name);
            } else if (type == 4) {
                name.erase(name.size()-4, 4);
                Img::New(file, n.g(name), fsize);
            } else die();
        }
        for (string str : dirs) {
            Directory(n.g(str), file, fileStart);
        }
    }

    void Load(string name) {
        WZ.InitTop(name);
        File(WZ);
        Img::ParseAll();
    }

    void Init() {
        GenKeys();
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
}