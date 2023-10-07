#include <stdio.h>
#include "headers/MovePreGen.h"
#include "headers/movegen.h"
#include "headers/helpers.h"

#pragma region Mask-Gen Code



// Maps the values of the int (originally ubyte) to the 1s of the mask
// Might not be the best code, was written by GPT-4 in C#
Bitboard MapIntToBitboardMask(int small, Bitboard mask)
{
    Bitboard result = 0;

    while (mask != 0)
    {
        Bitboard lsb = mask & (~mask + 1); // Extract least significant bit of mask, ulong fix for mask & -mask
        if ((small & 1) != 0)  // Check if the least significant bit of small is set
        {
            result |= lsb;  // Set the corresponding bit in result
        }
        mask ^= lsb;  // Remove the least significant bit from mask
        small >>= 1;  // Move to the next bit in small
    }

    return result;
}


// Get all possible constellations of Blockers
Bitboard* GetAllBlockerPositions(Bitboard blockerMask, int maxCount)
{
    Bitboard* blockerPositions = (Bitboard*) malloc((1 << maxCount) * sizeof(Bitboard));
    
    for (int i = 0; i <= (1 << maxCount); ++i)
    {
        Bitboard blockerPosition = MapIntToBitboardMask(i, blockerMask);
        blockerPositions[i] = blockerPosition;
    }

    return blockerPositions;
}

// Rook stuff
Bitboard RookMask(int sq)
{
    Bitboard result = 0;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1; r <= 6; r++) result |= (1UL << (fl + r * 8));
    for (r = rk - 1; r >= 1; r--) result |= (1UL << (fl + r * 8));
    for (f = fl + 1; f <= 6; f++) result |= (1UL << (f + rk * 8));
    for (f = fl - 1; f >= 1; f--) result |= (1UL << (f + rk * 8));
    return result;
}

Bitboard RookAttacks(int sq, Bitboard block)
{
    Bitboard result = 0UL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1; r <= 7; r++)
    {
        result |= (1UL << (fl + r * 8));
        if (block & (1UL << (fl + r * 8))) break;
    }
    for (r = rk - 1; r >= 0; r--)
    {
        result |= (1UL << (fl + r * 8));
        if (block & (1UL << (fl + r * 8))) break;
    }
    for (f = fl + 1; f <= 7; f++)
    {
        result |= (1UL << (f + rk * 8));
        if (block & (1UL << (f + rk * 8))) break;
    }
    for (f = fl - 1; f >= 0; f--)
    {
        result |= (1UL << (f + rk * 8));
        if (block & (1UL << (f + rk * 8))) break;
    }
    return result;
}

// Bishop stuff
Bitboard BishopMask(int sq)
{
    Bitboard result = 0;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1, f = fl + 1; r <= 6 && f <= 6; r++, f++) result |= (1UL << (f + r * 8));
    for (r = rk + 1, f = fl - 1; r <= 6 && f >= 1; r++, f--) result |= (1UL << (f + r * 8));
    for (r = rk - 1, f = fl + 1; r >= 1 && f <= 6; r--, f++) result |= (1UL << (f + r * 8));
    for (r = rk - 1, f = fl - 1; r >= 1 && f >= 1; r--, f--) result |= (1UL << (f + r * 8));
    return result;
}

Bitboard BishopAttacks(int sq, Bitboard block)
{
	Bitboard result = 0UL;
	int rk = sq / 8, fl = sq % 8, r, f;
	for (r = rk + 1, f = fl + 1; r <= 7 && f <= 7; r++, f++)
	{
		result |= (1UL << (f + r * 8));
		if (block & (1UL << (f + r * 8))) break;
	}
	for (r = rk + 1, f = fl - 1; r <= 7 && f >= 0; r++, f--)
	{
		result |= (1UL << (f + r * 8));
		if (block & (1UL << (f + r * 8))) break;
	}
	for (r = rk - 1, f = fl + 1; r >= 0 && f <= 7; r--, f++)
	{
		result |= (1UL << (f + r * 8));
		if (block & (1UL << (f + r * 8))) break;
	}
	for (r = rk - 1, f = fl - 1; r >= 0 && f >= 0; r--, f--)
	{
		result |= (1UL << (f + r * 8));
		if (block & (1UL << (f + r * 8))) break;
	}
	return result;
}

#pragma endregion

void PreGenerateAllPossibleMoves()
{
    printf("Pre-generating all possible moves\n");
    PreGenerateAllLegalKingMoves();
    PreGenerateAllLegalKnightMoves();

    // Sliding Pieces
    PreGenerateAllLegalRookMoves();
    //PreGenerateAllLegalBishopMoves();
}

static void PreGenerateAllLegalKingMoves()
{
    printf("Pre-generating all possible King moves\n");

    for(byte square = 0; square < 64; square++)
    {
        byte rank = (byte)(square >> 3), file = (byte)(square & 0x7);
        if (rank != 0)
        {
            KingMoves[square] |= 1UL << (square - 8);
            if (file != 0) KingMoves[square] |= 1UL << (square - 9);
            if (file != 7) KingMoves[square] |= 1UL << (square - 7);
        }
        if (rank != 7)
        {
            KingMoves[square] |= 1UL << (square + 8);
            if (file != 0) KingMoves[square] |= 1UL << (square + 7);
            if (file != 7) KingMoves[square] |= 1UL << (square + 9);
        }
        if (file != 0) KingMoves[square] |= 1UL << (square - 1);
        if (file != 7) KingMoves[square] |= 1UL << (square + 1);
    }
    printf("Done.\n");
}

static void PreGenerateAllLegalKnightMoves()
{
    printf("Pre-generating all possible Knight moves\n");
    
    for(byte square = 0; square < 64; square++)
    {
        KnightMoves[square] = 0;
        int rank = square / 8, file = square % 8;
        if (rank != 0)
        {
            if (file > 1) KnightMoves[square] |= 1UL << (square - 10);
            if (file < 6) KnightMoves[square] |= 1UL << (square - 6);
            if (rank > 1)
            {
                if (file != 0) KnightMoves[square] |= 1UL << (square - 17);
                if (file != 7) KnightMoves[square] |= 1UL << (square - 15);
            }
        }
        if (rank != 7)
        {
            if (file > 1) KnightMoves[square] |= 1UL << (square + 6);
            if (file < 6) KnightMoves[square] |= 1UL << (square + 10);
            if (rank < 6)
            {
                if (file != 0) KnightMoves[square] |= 1UL << (square + 15);
                if (file != 7) KnightMoves[square] |= 1UL << (square + 17);
            }
        }
    }
    printf("Done.\n");
}

static void PreGenerateAllLegalRookMoves()
{
    printf("Pre-generating all possible Rook moves\n");

    for(byte square = 0; square < 64; square++)
    {
        // Memory already allocated in memory.c
        Bitboard* moves = RookMoves[square]; //(Bitboard*) malloc(RookArraySizes[square] * sizeof(Bitboard));
        Bitboard mask = RookMask(square);
        RookFullBlockerMasks[square] = mask;
        int maxCount = Count_1s(mask);
        Bitboard* allBlockers = (Bitboard*) GetAllBlockerPositions(mask, maxCount);

        for(short block = 0; block < (1 << maxCount); block++)
        {
            byte hash = TranslateRook(square, allBlockers[block]);
            moves[hash] = RookAttacks(square, allBlockers[block]);
        }

        //RookMoves[square] = moves;
        free(allBlockers);
        // Dont free() moves, obviously
    }
    printf("Done.\n");
}

static void PreGenerateAllLegalBishopMoves()
{
    printf("Pre-generating all possible Bishop moves\n");

    for(byte square = 0; square < 64; square++)
    {
        // Memory already allocated (/freed) in memory.c
        Bitboard* moves = BishopMoves[square]; //(Bitboard*) malloc(BishopArraySizes[square] * sizeof(Bitboard));
        Bitboard mask = BishopMask(square);
        BishopFullBlockerMasks[square] = mask;
        int maxCount = Count_1s(mask);
        Bitboard* allBlockers = (Bitboard*) GetAllBlockerPositions(mask, maxCount);

        for(short block = 0; block < (1 << maxCount); block++)
        {
            byte hash = TranslateBishop(square, allBlockers[block]);
            moves[hash] = BishopAttacks(square, allBlockers[block]);
        }

        free(allBlockers);
    }
    printf("Done.\n");
}