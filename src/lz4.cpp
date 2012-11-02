/*
    LZ4 - Fast LZ compression algorithm
    Header File
    Copyright (C) 2011-2012, Yann Collet.
    BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

        * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following disclaimer
    in the documentation and/or other materials provided with the
    distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    You can contact the author at :
    - LZ4 homepage : http://fastcompression.blogspot.com/p/lz4.html
    - LZ4 source repository : http://code.google.com/p/lz4/
*/
// Modified by Peter Atechian
#include <cstdint>
#include <cassert>
#include "lz4.hpp"
namespace {
    const size_t copylength = 8;
    const size_t mlbits = 4;
    const size_t mlmask = (1 << mlbits) - 1;
    const size_t runbits = 4;
    const size_t runmask = (1 << runbits) - 1;
    const size_t stepsize = sizeof(size_t);
    const bool arch64 = stepsize == 8;
    const size_t dectable1[] = {0, 3, 2, 3, 0, 0, 0, 0};
    const size_t dectable2[] = {0, 0, 0, -1, 0, 1, 2, 3};
}
namespace LZ4 {
    void Uncompress(const uint8_t * source, uint8_t * dest, size_t osize) {
        const uint8_t * ip = source;
        uint8_t * op = dest;
        const uint8_t * const oend = op + osize;
        for (;;) {
            size_t length, len;
            uint8_t * opc;
            const uint8_t * ipc;
            const size_t token = *ip++;
            length = token >> mlbits;
            if (length == runmask) do {
                len = *ip++;
                length += len;
            } while (len == 255);
            opc = op + length;
            assert(opc <= oend);
            ipc = ip + length;
            do {
                *reinterpret_cast<size_t *>(op) = *reinterpret_cast<const size_t *>(ip);
                op += stepsize, ip += stepsize;
            } while (op < opc);
            op = opc;
            ip = ipc;
            if (op > oend - copylength) return;
            const uint8_t * ref = op - *reinterpret_cast<const uint16_t *>(ip);
            assert(ref >= dest);
            ip += 2;
            length = token & mlmask;
            if (length == mlmask) do {
                len = *ip++;
                length += len;
            } while (len == 255);
            if (op - ref < stepsize) {
                const size_t dec2 = arch64?dectable2[op - ref]:0;
                op[0] = ref[0];
                op[1] = ref[1];
                op[2] = ref[2];
                op[3] = ref[3];
                op += 4;
                ref += 4;
                ref -= dectable1[op - ref];
                *reinterpret_cast<uint32_t *>(op) = *reinterpret_cast<const uint32_t *>(ref);
                op += stepsize - 4;
                ref -= dec2;
            } else {
                *reinterpret_cast<size_t *>(op) = *reinterpret_cast<const size_t *>(ref);
                op += stepsize;
                ref += stepsize;
            }
            length -= stepsize - 4;
            opc = op + length;
            assert(opc <= oend);
            do {
                *reinterpret_cast<size_t *>(op) = *reinterpret_cast<const size_t *>(ref);
                op += stepsize, ref += stepsize;
            } while (op < opc);
            op = opc;
            assert(op != oend);
        }
    }
}