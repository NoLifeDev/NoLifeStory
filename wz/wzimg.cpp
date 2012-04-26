#include "wz.h"
#include "wzmain.h"
#include "wzimg.h"
namespace WZ {
    uint8_t Img::data[0x4000000];
    vector<Img> Img::Imgs;
//
//    void Img::ExtendedProperty(Node n) {
//        string name;
//        uint8_t a = Read<uint8_t>(file);
//        if (a == 0x1B) {
//            int32_t inc = Read<int32_t>(file);
//            uint32_t pos = offset+inc;
//            streamoff p = file->tellg();
//            file->seekg(pos);
//            name = ReadEncString(file);
//            file->seekg(p);
//        } else {
//            name = ReadEncString(file);
//        }
//        if (name == "Property") {
//            file->ignore(2);
//            SubProperty(n);
//        } else if (name == "Canvas") {
//            file->ignore(1);
//            uint8_t b = Read<uint8_t>(file);
//            if (b == 1) {
//                file->ignore(2);
//                SubProperty(n);
//            }
//            new PNGProperty(file, n);
//        } else if (name == "Shape2D#Vector2D") {
//            n.g("x").Set(ReadCInt(file));
//            n.g("y").Set(ReadCInt(file));
//        } else if (name == "Shape2D#Convex2D") {
//            int32_t ec = ReadCInt(file);
//            for (int i = 0; i < ec; ++i) ExtendedProperty(n.g(to_string(ec)));
//        } else if (name == "Sound_DX8") {
//            new SoundProperty(file, n);
//        } else if (name == "UOL") {
//            file->ignore(1);
//            uint8_t b = Read<uint8_t>(file);
//            switch (b) {
//            case 0:
//                n.g(name).Set(ReadEncString(file));
//                break;
//            case 1:
//                {
//                    uint32_t off = Read<uint32_t>(file);
//                    n.g(name).Set(ReadStringOffset(file, offset+off));
//                    break;
//                }
//            default:
//                die();
//            }
//        } else {
//            die();
//            return;
//        };
//    };
//
//    void Img::SubProperty(Node n) {
//        int32_t count = ReadCInt(file);
//        for (int i = 0; i < count; ++i) {
//            string name = ReadString(file, offset);
//            uint8_t a = Read<uint8_t>(file);
//            switch (a) {
//            case 0x00:
//                n.g(name).Set(i);
//                break;
//            case 0x0B:
//            case 0x02:
//                n.g(name).Set(Read<uint16_t>(file));
//                break;
//            case 0x03:
//                n.g(name).Set(ReadCInt(file));
//                break;
//            case 0x04:
//                if (Read<uint8_t>(file) == 0x80) n.g(name).Set(Read<float>(file));
//                break;
//            case 0x05:
//                n.g(name).Set(Read<double>(file));
//                break;
//            case 0x08:
//                n.g(name).Set(ReadString(file, offset));
//                break;
//            case 0x09:
//                {
//                    streamoff temp = Read<uint32_t>(file);
//                    temp += file->tellg();
//                    ExtendedProperty(n.g(name));
//                    file->seekg(temp);
//                    break;
//                }
//            default:
//                die();
//                return;
//            }
//        }
//    };

    void Img::ParseAll() {
        for (Img& img : Imgs) {
            img.Parse();
        }
        Imgs.clear();
        WZ.Recurse();
    }

    void Img::Parse() {
        clearerr(file);
        if (size > 0x4000000) die();
        fread(data, 1, size, file);
        //cout << n.Name() << ".img" << endl;
        return;
        /*
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
        SubProperty(n);
        delete this;*/
    }
//
//    PNGProperty::PNGProperty(ifstream* file, Node n) {
//        this->file = file;
//        //sprite.data = new SpriteData;
//        //sprite.data->loaded = false;
//        //sprite.data->width = ReadCInt(file);
//        //sprite.data->height = ReadCInt(file);
//        //sprite.data->png = this;
//        //sprite.data->originx = n["origin"]["x"];
//        //sprite.data->originy = n["origin"]["y"];
//        //n.Set(sprite);
//        format = ReadCInt(file);
//        format2 = Read<uint8_t>(file);
//        file->ignore(4);
//        length = Read<int32_t>(file);
//        if (length <= 0) die();
//        offset = (uint32_t)file->tellg();
//        offset++;
//    }
//
//    void PNGProperty::Parse() {
//        /*static uint8_t Buf1[0x1000000];
//        static uint8_t Buf2[0x1000000];
//        uint8_t* Src = Buf1;
//        uint8_t* Dest = Buf2;
//        bool abort = false;
//        auto Swap = [&]() {
//            swap(Src, Dest);
//        };
//        auto DecompressBlock = [&](uint32_t len) {
//            static z_stream strm;
//            strm.next_in = Src;
//            strm.avail_in = len;
//            strm.opaque = nullptr;
//            strm.zfree = nullptr;
//            strm.zalloc = nullptr;
//            inflateInit(&strm);
//            strm.next_out = Dest;
//            strm.avail_out = 0x1000000;
//            int err = inflate(&strm, Z_FINISH);
//            if (err != Z_BUF_ERROR) {
//                cerr << "Unexpected error from zlib: " << err << endl;
//                abort = true;
//                return;
//            }
//            inflateEnd(&strm);
//            Swap();
//        };
//        auto Decompress = [&](uint32_t len) {
//            if (Src[0] == 0x78 and Src[1] == 0x9C) {
//                DecompressBlock(len);
//            } else {
//                int i, p;
//                for (p = 0, i = 0; i < len-1;) {
//                    uint32_t blen = *(uint32_t*)&Src[i];
//                    i += 4;
//                    if (i+blen > len) {
//                        abort = true;
//                        return;
//                    }
//                    for (int j = 0; j < blen; j++) {
//                        Dest[p+j] = Src[i+j]^WZKey[j];
//                    }
//                    i += blen;
//                    p += blen;
//                }
//                Swap();
//                DecompressBlock(p);
//                if (abort) return;
//            }
//        };
//        if (file->fail()) {
//            die();
//            file->clear();
//        }
//        file->seekg(offset);
//        file->read((char*)Dest, length);
//        Swap();
//        int32_t f = format+format2;
//        GLsizei ww = sprite.data->width;
//        GLsizei hh = sprite.data->height;
//        GLsizei w = ww;
//        GLsizei h = hh;
//        if (!Graphics::NPOT) {
//            w = pot(w);
//            h = pot(h);
//        }
//        sprite.data->fw = w;
//        sprite.data->fh = h;
//        auto Resize = [&](int f) {
//            memset(Dest, 0, w*h*f);
//            for (int i = 0; i < hh; i++) {
//                memcpy(&Dest[i*w*f], &Src[i*ww*f], ww*f);
//            }
//            Swap();
//        };
//        Decompress(length);
//        if (abort) {
//            sprite.data->texture = 0;
//            sprite.data->png = nullptr;
//            delete this;
//            return;
//        }
//        glGenTextures(1, &sprite.data->texture);
//        glBindTexture(GL_TEXTURE_2D, sprite.data->texture);
//        auto SetTex = [&](GLenum type){glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA, type, Src);};
//        switch (f) {
//        case 1:
//            if (ww%2 and Graphics::NPOT or Graphics::Shit) {
//                uint32_t len = 2*ww*hh;
//                for (uint32_t i = 0; i < len; i++) {
//                    Dest[i*2] = (Src[i]&0x0F)*0x11;
//                    Dest[i*2+1] = ((Src[i]&0xF0)>>4)*0x11;
//                }
//                Swap();
//                if (!Graphics::NPOT) {
//                    Resize(4);
//                }
//                SetTex(GL_UNSIGNED_BYTE);
//            } else {
//                if (!Graphics::NPOT) {
//                    Resize(2);
//                }
//                SetTex(GL_UNSIGNED_SHORT_4_4_4_4_REV);
//            }
//            break;
//        case 2:
//            if (!Graphics::NPOT) {
//                Resize(4);
//            }
//            SetTex(GL_UNSIGNED_BYTE);
//            break;
//        case 513:
//            if (!Graphics::NPOT) {
//                Resize(2);
//            }
//            SetTex(GL_UNSIGNED_SHORT_5_6_5_REV);
//            break;
//        case 517:
//            if (pot(ww) != ww or pot(hh) != hh) {
//                cerr << "Non-square type 517 sprite found" << endl;
//                throw(273);
//            }
//            w >>= 4;
//            h >>= 4;
//            SetTex(GL_UNSIGNED_SHORT_5_6_5_REV);
//            break;
//        default:
//            cerr << "Unknown sprite format " << f << endl;
//            throw(273);
//        }
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//        sprite.data->loaded = true;*/
//    }
//
//    SoundProperty::SoundProperty(ifstream* file, Node n) {
//        this->file = file;
//        file->ignore(1);
//        len = ReadCInt(file);
//        ReadCInt(file);
//        offset = (uint32_t)file->tellg();
//        offset += 82;
//        data = 0;
//        //n.Set(Sound(this));
//    }
//
//    uint32_t SoundProperty::GetStream(bool loop) {
//        if (!data) {
//            file->seekg(offset);
//            data = new uint8_t[len];
//            file->read((char*)data, len);
//        }
//        //return BASS_StreamCreateFile(true, data, 0, len, BASS_SAMPLE_FLOAT|(loop?BASS_SAMPLE_LOOP:0));
//        return 0;
//    }
}