///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

namespace WZ {
    extern uint8_t *Key;
    extern uint8_t *AKey;
    extern uint16_t *WKey;
    extern int16_t EncVersion;
    extern uint32_t VersionHash;
    extern uint8_t Keys[3][0x10000];
    extern uint8_t AKeys[3][0x10000];
    extern uint16_t WKeys[3][0x8000];
    const uint32_t OffsetKey = 0x581C3F6D;
    void GenKeys();
}