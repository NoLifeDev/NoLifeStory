///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include "wzmain.h"

namespace WZ {
    uint8_t *Key = 0;
    uint8_t *AKey = 0;
    uint16_t *WKey = 0;
    int16_t EncVersion;
    uint16_t Version = 0;
    uint32_t VersionHash;
    uint8_t Keys[3][0x10000];
    uint16_t WKeys[3][0x8000];
    uint8_t AKeys[3][0x10000];
    AES AESGen;
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
        __m128i amask = _mm_setr_epi8(0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9);
        __m128i aplus = _mm_set1_epi8(0x10);
        __m128i *ak0 = (__m128i*)AKeys[0], *ak1 = (__m128i*)AKeys[1], *ak2 = (__m128i*)AKeys[2];
        __m128i wmask = _mm_setr_epi16(0xAAAA, 0xAAAB, 0xAAAC, 0xAAAD, 0xAAAE, 0xAAAF, 0xAAA0, 0xAAA1);
        __m128i wplus = _mm_set1_epi16(0x0008);
        __m128i *wk0 = (__m128i*)WKeys[0], *wk1 = (__m128i*)WKeys[1], *wk2 = (__m128i*)WKeys[2];
        __m128i *k0 = (__m128i*)Keys[0], *k1 = (__m128i*)Keys[1], *k2 = (__m128i*)Keys[2];
        for (int i = 0x1000; i > 0; --i) {
            _mm_store_si128(ak0, _mm_xor_si128(amask, _mm_load_si128(k0)));
            _mm_store_si128(ak1, _mm_xor_si128(amask, _mm_load_si128(k1)));
            _mm_store_si128(ak2, _mm_xor_si128(amask, _mm_load_si128(k2)));
            _mm_store_si128(wk0, _mm_xor_si128(wmask, _mm_load_si128(k0)));
            _mm_store_si128(wk1, _mm_xor_si128(wmask, _mm_load_si128(k1)));
            _mm_store_si128(wk2, _mm_xor_si128(wmask, _mm_load_si128(k2)));
            ++ak0, ++ak1, ++ak2;
            ++wk0, ++wk1, ++wk2;
            ++k0, ++k1, ++k2;
            amask = _mm_add_epi8(amask, aplus);
            wmask = _mm_add_epi8(wmask, wplus);
        }
        Key = Keys[0];
        AKey = AKeys[0];
        WKey = WKeys[0];
    }
}