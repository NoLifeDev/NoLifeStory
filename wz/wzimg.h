namespace WZ {
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
        static void New(FILE* file, Node n, uint32_t size) {
            Imgs.emplace_back(file, n, size);
        }
        static void ParseAll();
        Img(FILE* file, Node n, uint32_t size)
            :file(file), n(n), size(size), filepos(0) {}
    private:
        void Parse();
        Node n;
        FILE* file;
        uint32_t filepos;
        uint32_t size;
        void ExtendedProperty(Node n);
        void SubProperty(Node n);
        static vector<Img> Imgs;
        static uint8_t data[0x4000000];
        template <class T>
        T Read() {
            filepos += sizeof(T);
        }
    };
    
}