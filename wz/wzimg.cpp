///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include "wzmain.h"

namespace WZ {
    vector<Img*> Imgs;
    atomic_flag flag;
    void Img::ExtendedProperty(Node n) {
        string name = file.ReadTypeString();
        if (name == "Property") {
            file.Skip(2);
            SubProperty(n);
        } else if (name == "Canvas") {
            file.Skip(1);
            uint8_t b = file.Read<uint8_t>();
            if (b == 1) {
                file.Skip(2);
                SubProperty(n);
            }
            //new PNGProperty(file, n, offset);
        } else if (name == "Shape2D#Vector2D") {
            n.Reserve(2);
            n.g("x", 0).Set(file.ReadCInt());
            n.g("y", 1).Set(file.ReadCInt());
        } else if (name == "Shape2D#Convex2D") {
            int32_t ec = file.ReadCInt();
            n.Reserve(ec);
            for (int i = 0; i < ec; ++i) ExtendedProperty(n.g(to_string(ec), i));
        } else if (name == "Sound_DX8") {
            //new SoundProperty(file, n, offset);
        } else if (name == "UOL") {
            file.Skip(1);
            n.SetUOL(file.ReadTypeString());
        } else {
            die();
            return;
        };
    };

    void Img::SubProperty(Node n) {
        int32_t count = file.ReadCInt();
        n.Reserve(count);
        for (int i = 0; i < count; ++i) {
            string name = file.ReadTypeString();
            uint8_t a = file.Read<uint8_t>();
            switch (a) {
            case 0x00:
                n.g(name, i).Set(i);
                break;
            case 0x0B:
            case 0x02:
                n.g(name, i).Set(file.Read<uint16_t>());
                break;
            case 0x03:
                n.g(name, i).Set(file.ReadCInt());
                break;
            case 0x04:
                if (file.Read<uint8_t>() == 0x80) n.g(name, i).Set(file.Read<float>());
                break;
            case 0x05:
                n.g(name, i).Set(file.Read<double>());
                break;
            case 0x08:
                n.g(name, i).Set(file.ReadTypeString());
                break;
            case 0x09:
                {
                    uint32_t p = file.Read<uint32_t>();
                    p += file.Tell();
                    ExtendedProperty(n.g(name, i));
                    file.Seek(p);
                    break;
                }
            default:
                die();
                return;
            }
        }
    };

    void Img::Parse() {
        file.Map(offset, size);
        uint8_t a = file.Read<uint8_t>();
        if (a != 0x73) {
            die();
            return;
        }
        string s = file.ReadEncString();
        if (s != "Property") {
            die();
            return;
        }
        uint16_t b = file.Read<uint16_t>();
        if (b != 0) {
            die();
            return;
        }
        SubProperty(n);
        file.Unmap();
        delete this;
    }

    PNGProperty::PNGProperty(MapFile file, Node n, uint32_t off) {
        this->file = file;
        //sprite.data = new SpriteData;
        //sprite.data->loaded = false;
        //sprite.data->width = ReadCInt(file);
        file.ReadCInt();
        //sprite.data->height = ReadCInt(file);
        file.ReadCInt();
        //sprite.data->png = this;
        //sprite.data->originx = n["origin"]["x"];
        //sprite.data->originy = n["origin"]["y"];
        //n.Set(sprite);
        format = file.ReadCInt();
        format2 = file.Read<uint8_t>();
        file.Skip(4);
        length = file.Read<int32_t>();
        if (length <= 0) die();
        offset = file.Tell()+off+1;
    }
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
    SoundProperty::SoundProperty(MapFile file, Node n, uint32_t off) {
        this->file = file;
        file.Skip(1);
        len = file.ReadCInt();
        file.ReadCInt();
        offset = file.Tell()+off+82;
        data = 0;
        //n.Set(Sound(this));
    }
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