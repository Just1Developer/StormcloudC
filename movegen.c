#include "headers/movegen.h"
#include "headers/magics.h"

int TranslateRook(byte square, Bitboard BlockerBitboard)
{
    return (int) ((BlockerBitboard * RookMagics[square]) >> (64 - RookBits[square]));
}

int TranslateBishop(byte square, Bitboard BlockerBitboard)
{
    return (int) ((BlockerBitboard * BishopMagics[square]) >> (64 - BishopBits[square]));
}