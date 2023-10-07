#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "constants.h"

extern int TranslateRook(byte square, Bitboard BlockerBitboard);
extern int TranslateBishop(byte square, Bitboard BlockerBitboard);

#endif