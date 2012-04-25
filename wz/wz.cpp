///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include "wz.h"
#include "aes.h"

namespace WZ {
    void die() {
        throw(273);
    }
    path Path;
    Node WZ;
    vector<path> Paths;
    uint8_t *Key = 0;
    int16_t EncVersion;
    uint16_t Version = 0;
    uint32_t VersionHash;

#pragma region WZ Keys
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
#pragma endregion

#pragma region Property classes
    class PNGProperty {
    public:
        PNGProperty(ifstream* file, Node n);
        void Parse();
    private:
        ifstream* file;
        //Sprite sprite;
        int32_t format;
        int32_t length;
        uint32_t offset;
        uint8_t format2;
    };
    class SoundProperty {
    public:
        SoundProperty(ifstream* file, Node n);
        uint32_t GetStream(bool loop);
    private:
        int32_t len;
        uint8_t* data;
        uint32_t offset;
        ifstream* file;
    };
    class Img {
    public:
        Img(ifstream* file, Node n, uint32_t offset);
        void Parse();
    private:
        Node n;
        uint32_t offset;
        ifstream* file;
    };
#pragma endregion

#pragma region File reading stuff
    template <class T>
    T Read(ifstream* file) {
        T v;
        file->read((char*)&v, sizeof(v));
        return v;
    }

    int32_t ReadCInt(ifstream* file) {
        int8_t a = Read<int8_t>(file);
        if (a != -128) return a;
        else {
            int32_t b = Read<int32_t>(file);
            return b;
        }
    }

    string ReadEncString(ifstream* file) {
        int8_t slen = Read<int8_t>(file);
        if (slen == 0) return string();
        else if (slen > 0) {
            int32_t len;
            if (slen == 127) len = Read<int32_t>(file);
            else len = slen;
            if (len <= 0) return string();
            uint16_t mask = 0xAAAA;
            wstring ws(len, '0');
            file->read((char*)&ws[0], 2*len);
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
            file->read(&s[0], len);
            for (int i = 0; i < len; ++i) {
                s[i] ^= mask;
                s[i] ^= Key[i];
                mask++;
            }
            return s;
        }
    }

    void ReadEncFast(ifstream* file) {
        int8_t slen = Read<int8_t>(file);
        if (slen == 0) return;
        if (slen > 0) {
            int32_t len;
            if (slen == 127) len = Read<int32_t>(file);
            else len = slen;
            if (len <= 0) return;
            file->ignore(len*2);
        } else {
            int32_t len;
            if (slen == -128) len = Read<int32_t>(file);
            else len = -slen;
            if (len <= 0) return;
            file->ignore(len);
        }
    }

    string ReadString(ifstream* file, uint32_t offset) {
        uint8_t a = Read<uint8_t>(file);
        switch (a) {
        case 0x00:
        case 0x73:
            return ReadEncString(file);
        case 0x01:
        case 0x1B:
            {
                offset += Read<int32_t>(file);
                streamoff p = file->tellg();
                file->seekg(offset);
                string s = ReadEncString(file);
                file->seekg(p);
                return s;
            }
        default:
            return string();
        }
    }

    string ReadStringOffset(ifstream* file, uint32_t offset) {
        streamoff p = file->tellg();
        file->seekg(offset);
        string s = ReadEncString(file);
        file->seekg(p);
        return s;
    }
    uint32_t ReadOffset(ifstream* file, uint32_t fileStart) {
        uint32_t p = (uint32_t)file->tellg();
        p = (p-fileStart)^0xFFFFFFFF;
        p *= VersionHash;
        p -= OffsetKey;
        p = (p<<(p&0x1F))|(p>>(32-p&0x1F));
        uint32_t more = Read<uint32_t>(file);
        p ^= more;
        p += fileStart*2;
        return p;
    }
    #pragma endregion
    
    #pragma region WZ Parsing
    void Directory(Node n, ifstream* file, uint32_t fileStart);

    void File(Node n) {
        path filename = Path/path(n.Name()+".wz");
        if (!exists(filename)) die();
        ifstream *file = new ifstream(filename, ios::in|ios::binary);
        if (!file->is_open()) die();
        char ident[4];
        file->read(ident, 4);
        if (string(ident, 4) != "PKG1") die();
        uint64_t fileSize = Read<uint64_t>(file);
        uint32_t fileStart = Read<uint32_t>(file);
        string copyright;
        getline(*file, copyright, '\0');
        file->seekg(fileStart);
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
                    c = (uint32_t)file->tellg();
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
                        file->clear();
                        file->seekg(c);
                        uint32_t offset = ReadOffset(file, fileStart);
                        if (offset > fileSize) continue;
                        file->seekg(offset);
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
            file->seekg(fileStart+2);
        } else {
            int16_t eversion = Read<int16_t>(file);
            if (eversion != EncVersion) die();
        }
        if (ReadCInt(file) == 0) die();
        file->seekg(fileStart+2);
        Directory(n, file, fileStart);
    }

    void Directory(Node n, ifstream* file, uint32_t fileStart) {
        int32_t count = ReadCInt(file);
        if (count == 0) {
            File(n);
            return;
        }
        set<pair<string, uint32_t>> dirs;
        for (int i = 0; i < count; ++i) {
            string name;
            uint8_t type = Read<uint8_t>(file);
            if (type == 1) {
                file->ignore(10);
                die();
                continue;
            } else if (type == 2) {
                int32_t s = Read<int32_t>(file);
                streamoff p = file->tellg();
                file->seekg(fileStart+s);
                type = Read<uint8_t>(file);
                name = ReadEncString(file);
                file->seekg(p);
            } else if (type == 3) name = ReadEncString(file);
            else if (type == 4) name = ReadEncString(file);
            else die();
            int32_t fsize = ReadCInt(file);
            int32_t checksum = ReadCInt(file);
            uint32_t offset = ReadOffset(file, fileStart);
            if (type == 3) {
                dirs.insert(pair<string, uint32_t>(name, offset));
            } else if (type == 4) {
                name.erase(name.size()-4, 4);
                new Img(file, n.g(name), offset);
            } else die();
        }
        for (auto it = dirs.begin(); it != dirs.end(); it++) {
            file->seekg(it->second);
            Directory(n.g(it->first), file, fileStart);
        }
    }

    void Init() {
        GenKeys();
        for (unsigned int i = 0; i < Paths.size(); ++i) {
            Path = Paths[i];
            if (exists(Path/path("Data.wz"))) {
                WZ.InitTop("Data");
                File(WZ);
                return;
            }
            if (exists(Path/path("Base.wz"))) {
                WZ.InitTop("Base");
                File(WZ);
                return;
            }
        }
        die();
    }

    void SubProperty(ifstream* file, Node n, uint32_t offset);

    void ExtendedProperty(ifstream* file, Node n, uint32_t offset) {
            string name;
            uint8_t a = Read<uint8_t>(file);
            if (a == 0x1B) {
                int32_t inc = Read<int32_t>(file);
                uint32_t pos = offset+inc;
                streamoff p = file->tellg();
                file->seekg(pos);
                name = ReadEncString(file);
                file->seekg(p);
            } else {
                name = ReadEncString(file);
            }
            if (name == "Property") {
                file->ignore(2);
                SubProperty(file, n, offset);
            } else if (name == "Canvas") {
                file->ignore(1);
                uint8_t b = Read<uint8_t>(file);
                if (b == 1) {
                    file->ignore(2);
                    SubProperty(file, n, offset);
                }
                new PNGProperty(file, n);
            } else if (name == "Shape2D#Vector2D") {
                n.g("x").Set(ReadCInt(file));
                n.g("y").Set(ReadCInt(file));
            } else if (name == "Shape2D#Convex2D") {
                int32_t ec = ReadCInt(file);
                for (int i = 0; i < ec; ++i) ExtendedProperty(file, n.g(to_string(ec)), offset);
            } else if (name == "Sound_DX8") {
                new SoundProperty(file, n);
            } else if (name == "UOL") {
                file->ignore(1);
                uint8_t b = Read<uint8_t>(file);
                switch (b) {
                case 0:
                    n.g(name).Set(ReadEncString(file));
                    break;
                case 1:
                    {
                        uint32_t off = Read<uint32_t>(file);
                        n.g(name).Set(ReadStringOffset(file, offset+off));
                        break;
                    }
                default:
                    die();
                }
            } else {
                die();
                return;
            };
        };

    void SubProperty(ifstream* file, Node n, uint32_t offset) {
        int32_t count = ReadCInt(file);
        for (int i = 0; i < count; ++i) {
            string name = ReadString(file, offset);
            uint8_t a = Read<uint8_t>(file);
            switch (a) {
            case 0x00:
                n.g(name).Set(i);
                break;
            case 0x0B:
            case 0x02:
                n.g(name).Set(Read<uint16_t>(file));
                break;
            case 0x03:
                n.g(name).Set(ReadCInt(file));
                break;
            case 0x04:
                if (Read<uint8_t>(file) == 0x80) n.g(name).Set(Read<float>(file));
                break;
            case 0x05:
                n.g(name).Set(Read<double>(file));
                break;
            case 0x08:
                n.g(name).Set(ReadString(file, offset));
                break;
            case 0x09:
                {
                    streamoff temp = Read<uint32_t>(file);
                    temp += file->tellg();
                    ExtendedProperty(file, n.g(name), offset);
                    file->seekg(temp);
                    break;
                }
            default:
                die();
                return;
            }
        }
    };

    void Resolve(Node n) {
        if (n["UOL"]) {
            string s = n["UOL"];
            stringstream ss(s);
	        vector<string> parts;
            string st;
	        while (getline(ss, st, '/')) {
		        parts.push_back(st);
	        }
            Node nn = n[".."];
            for (string str : parts) {
                if (!nn) break;
                nn = nn[str];
            }
            if (nn) n.Assign(nn);
        } else {
            for (pair<string, Node> nn : n) if (nn.second) Resolve(nn.second);
        }
    };

    Img::Img(ifstream* file, Node n, uint32_t offset) {
        this->n = n;
        n.Set(this);
        this->offset = offset;
        this->file = file;
    }

    void Img::Parse() {
        cout << n.Name() << endl;
        if (file->fail()) {
            die();
            file->clear();
        }
        file->seekg(offset);
        uint8_t a = Read<uint8_t>(file);
        if (a != 0x73) {
            die();
            delete this;
            return;
        }
        string s = ReadEncString(file);
        if (s != "Property") {
            die();
            delete this;
            return;
        }
        uint16_t b = Read<uint16_t>(file);
        if (b != 0) {
            die();
            delete this;
            return;
        }
        SubProperty(file, n, offset);
        Resolve(n);
        delete this;
    }
    PNGProperty::PNGProperty(ifstream* file, Node n) {
        this->file = file;
        //sprite.data = new SpriteData;
        //sprite.data->loaded = false;
        //sprite.data->width = ReadCInt(file);
        //sprite.data->height = ReadCInt(file);
        //sprite.data->png = this;
        //sprite.data->originx = n["origin"]["x"];
        //sprite.data->originy = n["origin"]["y"];
        //n.Set(sprite);
        format = ReadCInt(file);
        format2 = Read<uint8_t>(file);
        file->ignore(4);
        length = Read<int32_t>(file);
        if (length <= 0) die();
        offset = (uint32_t)file->tellg();
        offset++;
    }

    void PNGProperty::Parse() {
        /*static uint8_t Buf1[0x1000000];
        static uint8_t Buf2[0x1000000];
        uint8_t* Src = Buf1;
        uint8_t* Dest = Buf2;
        bool abort = false;
        auto Swap = [&]() {
            swap(Src, Dest);
        };
        auto DecompressBlock = [&](uint32_t len) {
            static z_stream strm;
            strm.next_in = Src;
            strm.avail_in = len;
            strm.opaque = nullptr;
            strm.zfree = nullptr;
            strm.zalloc = nullptr;
            inflateInit(&strm);
            strm.next_out = Dest;
            strm.avail_out = 0x1000000;
            int err = inflate(&strm, Z_FINISH);
            if (err != Z_BUF_ERROR) {
                cerr << "Unexpected error from zlib: " << err << endl;
                abort = true;
                return;
            }
            inflateEnd(&strm);
            Swap();
        };
        auto Decompress = [&](uint32_t len) {
            if (Src[0] == 0x78 and Src[1] == 0x9C) {
                DecompressBlock(len);
            } else {
                int i, p;
                for (p = 0, i = 0; i < len-1;) {
                    uint32_t blen = *(uint32_t*)&Src[i];
                    i += 4;
                    if (i+blen > len) {
                        abort = true;
                        return;
                    }
                    for (int j = 0; j < blen; j++) {
                        Dest[p+j] = Src[i+j]^WZKey[j];
                    }
                    i += blen;
                    p += blen;
                }
                Swap();
                DecompressBlock(p);
                if (abort) return;
            }
        };
        if (file->fail()) {
            die();
            file->clear();
        }
        file->seekg(offset);
        file->read((char*)Dest, length);
        Swap();
        int32_t f = format+format2;
        GLsizei ww = sprite.data->width;
        GLsizei hh = sprite.data->height;
        GLsizei w = ww;
        GLsizei h = hh;
        if (!Graphics::NPOT) {
            w = pot(w);
            h = pot(h);
        }
        sprite.data->fw = w;
        sprite.data->fh = h;
        auto Resize = [&](int f) {
            memset(Dest, 0, w*h*f);
            for (int i = 0; i < hh; i++) {
                memcpy(&Dest[i*w*f], &Src[i*ww*f], ww*f);
            }
            Swap();
        };
        Decompress(length);
        if (abort) {
            sprite.data->texture = 0;
            sprite.data->png = nullptr;
            delete this;
            return;
        }
        glGenTextures(1, &sprite.data->texture);
        glBindTexture(GL_TEXTURE_2D, sprite.data->texture);
        auto SetTex = [&](GLenum type){glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA, type, Src);};
        switch (f) {
        case 1:
            if (ww%2 and Graphics::NPOT or Graphics::Shit) {
                uint32_t len = 2*ww*hh;
                for (uint32_t i = 0; i < len; i++) {
                    Dest[i*2] = (Src[i]&0x0F)*0x11;
                    Dest[i*2+1] = ((Src[i]&0xF0)>>4)*0x11;
                }
                Swap();
                if (!Graphics::NPOT) {
                    Resize(4);
                }
                SetTex(GL_UNSIGNED_BYTE);
            } else {
                if (!Graphics::NPOT) {
                    Resize(2);
                }
                SetTex(GL_UNSIGNED_SHORT_4_4_4_4_REV);
            }
            break;
        case 2:
            if (!Graphics::NPOT) {
                Resize(4);
            }
            SetTex(GL_UNSIGNED_BYTE);
            break;
        case 513:
            if (!Graphics::NPOT) {
                Resize(2);
            }
            SetTex(GL_UNSIGNED_SHORT_5_6_5_REV);
            break;
        case 517:
            if (pot(ww) != ww or pot(hh) != hh) {
                cerr << "Non-square type 517 sprite found" << endl;
                throw(273);
            }
            w >>= 4;
            h >>= 4;
            SetTex(GL_UNSIGNED_SHORT_5_6_5_REV);
            break;
        default:
            cerr << "Unknown sprite format " << f << endl;
            throw(273);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        sprite.data->loaded = true;*/
    }

    SoundProperty::SoundProperty(ifstream* file, Node n) {
        this->file = file;
        file->ignore(1);
        len = ReadCInt(file);
        ReadCInt(file);
        offset = (uint32_t)file->tellg();
        offset += 82;
        data = 0;
        //n.Set(Sound(this));
    }

    uint32_t SoundProperty::GetStream(bool loop) {
        if (!data) {
            file->seekg(offset);
            data = new uint8_t[len];
            file->read((char*)data, len);
        }
        //return BASS_StreamCreateFile(true, data, 0, len, BASS_SAMPLE_FLOAT|(loop?BASS_SAMPLE_LOOP:0));
        return 0;
    }
    #pragma endregion

    #pragma region Node Stuff
    class Node::Data {
    public:
        Data(const Node& parent, string name)
            :image(nullptr), intValue(0), floatValue(0), parent(parent), name(name) {}
        string stringValue;
        double floatValue;
        int intValue;
        //Sprite sprite;
        //Sound sound;
        Node parent;
        string name;
        Img* image;
        map <string, Node> children;
    private:
        Data()
            :image(nullptr), intValue(0), floatValue(0) {}
        Data(const Node::Data&);
    };

    Node::Node() : data(nullptr) {}

    Node::Node(const Node& other) : data(other.data) {}

    Node::Node(const Node& other, string name) : data(new Data(other, name)) {}

    Node& Node::operator= (const Node& other) {
        data = other.data;
        return *this;
    }

    Node Node::operator[] (const string key) const {
        if (!data) return Node();
        if (data->image) {
            Img* img = data->image;;
            data->image = nullptr;
            img->Parse();
        }
        if (key == "..") return data->parent;
        if (key == ".") return *this;
        auto n = data->children.find(key);
        if (n == data->children.end()) return Node();
        return n->second;
    }

    Node Node::operator[] (const char key[]) const {
        return (*this)[(string)key];
    }

    Node Node::operator[] (const int key) const {
        return (*this)[to_string(key)];
    }
    Node Node::operator[] (const Node key) const {
        return (*this)[(string)key];
    }

    Node Node::g(const string key) {
        return data->children.emplace(key, Node(*this, key)).first->second;
    }

    map<string, Node>::const_iterator Node::begin() const {
        if (!data) return map<string, Node>::const_iterator();
        return data->children.begin();
    }

    map<string, Node>::const_iterator Node::end() const {
        if (!data) return map<string, Node>::const_iterator();
        return data->children.end();
    }

    map<string, Node>::const_reverse_iterator Node::rbegin() const {
        if (!data) return map<string, Node>::const_reverse_iterator();
        return data->children.rbegin();
    }

    map<string, Node>::const_reverse_iterator Node::rend() const {
        if (!data) return map<string, Node>::const_reverse_iterator();
        return data->children.rend();
    }

    string Node::Name() const {
        if (!data) return string();
        return data->name;
    }

    void Node::InitTop(const string s) {
        data = new Data(Node(), s);
    }

    void Node::Assign(const Node other) {
        data = other.data;
    }

    Node::operator bool() const {
        return (bool)data;
    }

    Node::operator string() const {
        if (!data) return string();
        return data->stringValue;
    }

    Node::operator double() const {
        if (!data) return 0;
        return data->floatValue;
    }

    Node::operator int() const {
        if (!data) return 0;
        return data->intValue;
    }

    /*Node::operator Sprite() const {
        if (!data) return Sprite();
        return data->sprite;
    }

    Node::operator Sound() const {
        if (!data) return Sound();
        return data->sound;
    }*/

    void Node::Set(const string v) {
        //data->intValue = stoi(v);
        //data->floatValue = stod(v);
        data->intValue = 0;
        data->floatValue = 0;
        data->stringValue = v;
    }

    void Node::Set(const double v) {
        data->intValue = (int)v;
        data->floatValue = v;
        data->stringValue = to_string(v);
    }

    void Node::Set(const int v) {
        data->intValue = v;
        data->floatValue = v;
        data->stringValue = to_string(v);
    }

    /*void Node::Set(const Sprite& v) {
        data->sprite = v;
    }

    void Node::Set(const Sound& v) {
        data->sound = v;
    }*/

    void Node::Set(Img* v) {
        data->image = v;
    }
    void Node::Recurse() {
        if (!data) return;
        if (data->image) {
            Img* img = data->image;;
            data->image = nullptr;
            img->Parse();
            return;
        }
        for (pair<string, Node> n : data->children) {
            n.second.Recurse();
        }
    }
    #pragma endregion
}