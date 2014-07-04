///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

namespace WZ {
class PNGProperty {
public:
    PNGProperty(MapFile file, Node n, uint32_t off);
    void Parse();

private:
    MapFile file;
    // Sprite sprite;
    int32_t format;
    int32_t length;
    uint32_t offset;
    uint8_t format2;
};
class SoundProperty {
public:
    SoundProperty(MapFile file, Node n, uint32_t off);
    uint32_t GetStream(bool loop);

private:
    int32_t len;
    uint8_t *data;
    uint32_t offset;
    MapFile file;
};
class Img {
public:
    static vector<Img *> Imgs;
    Img(MapFile file, Node n, uint32_t size, uint32_t offset)
        : file(file), n(n), size(size), offset(offset) {
        if (!Lazy)
            Imgs.push_back(this);
        else
            n.Set(this);
    }
    void Parse();
    void *operator new(size_t);

private:
    Node n;
    MapFile file;
    uint32_t size;
    uint32_t offset;
    void ExtendedProperty(Node n);
    void SubProperty(Node n);
};
}