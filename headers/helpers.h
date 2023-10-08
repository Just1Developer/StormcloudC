#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include "types.h"

static int Count_1s(Bitboard b)
{
    int r;
    for (r = 0; b != 0; r++, b &= b - 1) ;
    return r;
}

// Trailing Zero Count (2 Approaches)

#if defined(__GNUC__) || defined(__clang__)
    #define USE_GCC_INTRINSICS
#elif defined(_MSC_VER)
    #define USE_MSVC_INTRINSICS
    #include <intrin.h>
#endif

static byte TrailingZeroCount(Bitboard value) {
    if (value == 0) return 64;

    #if defined(USE_GCC_INTRINSICS)
        return __builtin_ctzll(value);
    #elif defined(USE_MSVC_INTRINSICS)
        unsigned long index;
        _BitScanForward64(&index, value);
        return (byte)index;
    #else
        // Fall back to the De Bruijn method for other compilers
        return TrailingZeroCount_DeBruijn(value);
    #endif
}

// Provided By GPT-4
static byte TrailingZeroCount_DeBruijn(Bitboard value)
{
    // If value is 0, return 64.
    if (value == 0) {
        return 64;
    }

    // Use De Bruijn multiplication to map a bit position
    // to a De Bruijn sequence.
    static const byte Index64[64] = {
        0,  1,  2, 53,  3,  7, 54, 27,
        4, 38, 41,  8, 34, 55, 48, 28,
        62,  5, 39, 46, 44, 42, 22,  9,
        24, 35, 59, 56, 49, 18, 29, 11,
        63, 52,  6, 26, 37, 40, 33, 47,
        61, 45, 43, 21, 23, 58, 17, 10,
        51, 25, 36, 32, 60, 20, 57, 16,
        50, 31, 19, 15, 30, 14, 13, 12
    };
    static const Bitboard debruijn64 = 0x022fdd63cc95386d;

    return Index64[((value & -value) * debruijn64) >> 58];
}

static ushort Pack(byte FromSquare, int ToSquare, byte data)
{
    return (ushort)((FromSquare << 10) | (ToSquare << 4) | (data & 0xF));
}


static byte GetMoveSquareFrom(ushort move)
{
    return move >> 10;
}

static byte GetMoveSquareTo(ushort move)
{
    return (move >> 4) & BITMASK_SQUARE;
}

static byte GetMoveData(ushort move)
{
    return move & BITMASK_MOVEDATA;
}

// Bitboard[ FromSquareBitboard, FromSquareBitboard, MoveData (byte, but expand due)]
static Bitboard* Unpack(ushort packedMove)
{
    Bitboard* unpacked = malloc(3 * sizeof(Bitboard));
    unpacked[0] = 1UL << GetMoveSquareFrom(packedMove);
    unpacked[1] = 1UL << GetMoveSquareTo(packedMove);
    unpacked[2] = 1UL << GetMoveData(packedMove);
    return unpacked;
}

#endif