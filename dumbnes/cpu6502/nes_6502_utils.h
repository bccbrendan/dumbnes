#ifndef __NES_6502_UTILS_H
#define __NES_6502_UTILS_H
/*
 * nes_6502_utils.h
 *
 * simple helpers, inlines, etc.
 * brendan long
 * Feb 2015
 */
#include "dumbnes_includes.h"

namespace dumbnes { namespace cpu6502 {

    inline bool ByteNegative(uint8_t b) { return b >= 0x80; }

    inline uint8_t DecodeBCD(uint8_t hex_bcd)
    {
        return (10 * (hex_bcd >> 4)) + (hex_bcd & 0xF);
    }

    inline uint8_t EncodeBCD(uint8_t bcd)
    {
        return ((bcd / 10) << 4) | (bcd % 10);
    }
}}

#endif /* __NES_6502_UTILS_H */
