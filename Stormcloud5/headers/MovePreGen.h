#ifndef MOVEPREGEN_H
#define MOVEPREGEN_H

#include "memory.h"

extern void PreGenerateAllPossibleMoves();

static void PreGenerateAllLegalKingMoves();
static void PreGenerateAllLegalKnightMoves();
static void PreGenerateAllLegalRookMoves();
static void PreGenerateAllLegalBishopMoves();

#endif