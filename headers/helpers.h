#ifndef HELPERS_H
#define HELPERS_H

#include "constants.h"
static int Count_1s(Bitboard b)
{
    int r;
    for (r = 0; b != 0; r++, b &= b - 1) ;
    return r;
}

#endif