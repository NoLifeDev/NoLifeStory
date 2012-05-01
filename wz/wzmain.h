///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS 1
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <codecvt>
#include <sstream>
#include <iostream>
#include <thread>
#include <atomic>
#include <fstream>
using namespace std;
using namespace std::tr1;
using namespace std::tr2;
using namespace std::tr2::sys;
#include "aes.h"
#include "mapfile.h"
#include "wz.h"
#include "wzimg.h"
#include "wzcrypto.h"

static_assert(sizeof(wchar_t) == 2, "");
inline void die() {
    throw(273);
}

namespace WZ {
    extern bool Lazy;
}