#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include "types.h"

extern Bitboard** RookMoves;
extern Bitboard** BishopMoves;
static Bitboard KnightMoves[64];
static Bitboard KingMoves[64];
static Bitboard RookFullBlockerMasks[64];
static Bitboard BishopFullBlockerMasks[64];

extern void allocateMagicTables();
extern void freeMagicTables();

#endif