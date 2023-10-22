#ifndef MOVEGEN_ENGINE_H
#define MOVEGEN_ENGINE_H

#include "types.h"

extern byte GenerateAllMoves(Bitboard* myBitboards, Bitboard* opponentBitboards, ushort* moves, bool isWhite);
extern byte GenerateAllMoves_CapturesOnly(Bitboard* myBitboards, Bitboard* opponentBitboards, ushort* moves, bool isWhite);

#endif