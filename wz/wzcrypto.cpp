///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include "wzmain.h"

namespace WZ {
    uint8_t *Key = 0;
    int16_t EncVersion;
    uint16_t Version = 0;
    uint32_t VersionHash;
    uint8_t Keys[3][0x10000];
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
        Key = Keys[0];
    }
}