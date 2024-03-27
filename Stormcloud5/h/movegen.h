#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "types.h"

// extern?

extern int TranslateRook(byte square, Bitboard BlockerBitboard);
extern int TranslateBishop(byte square, Bitboard BlockerBitboard);

extern Bitboard GetQueenMoves(byte fromSquare, Bitboard myBitboardInverted, Bitboard CompleteGamestate);
extern Bitboard GetRookMoves(byte fromSquare, Bitboard myBitboardInverted, Bitboard CompleteGamestate);
extern Bitboard GetBishopMoves(byte fromSquare, Bitboard myBitboardInverted, Bitboard CompleteGamestate);
extern Bitboard GetKnightMoves(byte fromSquare, Bitboard myBitboardInverted);
extern Bitboard GetKingMoves(byte fromSquare, Bitboard myBitboardInverted);

#endif