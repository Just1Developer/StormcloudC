#include "headers/memory.h"
#include "headers/MovePreGen.h"
#include <stdio.h>

Bitboard** RookMoves;
Bitboard** BishopMoves;

void allocateMagicTables()
{
    RookMoves = (Bitboard**) malloc(BOARD_SIZE * sizeof(Bitboard*));
    BishopMoves = (Bitboard**) malloc(BOARD_SIZE * sizeof(Bitboard*));

    for(byte i = 0; i < BOARD_SIZE; ++i)
    {
        RookMoves[i] = (Bitboard*) malloc(RookArraySizes[i] * sizeof(Bitboard));
        BishopMoves[i] = (Bitboard*) malloc(BishopArraySizes[i] * sizeof(Bitboard));
    }
    printf("Allocated all precomputed magic tables\n");
}

void freeMagicTables()
{
    for(byte i = 0; i < BOARD_SIZE; ++i)
    {
        free(RookMoves[i]);
        free(BishopMoves[i]);
    }
    free(RookMoves);
    free(BishopMoves);
    printf("Freed all precomputed magic tables\n");
}