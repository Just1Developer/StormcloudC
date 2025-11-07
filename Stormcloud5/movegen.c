#include "headers/arrayhelpers.h"
#include "headers/magics.h"
#include "headers/memory.h"
#include "headers/movegen.h"

// Apply Magic

int TranslateRook(byte square, Bitboard BlockerBitboard)
{
    return (int) ((BlockerBitboard * RookMagics[square]) >> (64 - RookBits[square]));
}

int TranslateBishop(byte square, Bitboard BlockerBitboard)
{
    return (int) ((BlockerBitboard * BishopMagics[square]) >> (64 - BishopBits[square]));
}

// General Move Generation

Bitboard GetKnightMoves(byte fromSquare, Bitboard myBitboardInverted)
{
    return KnightMoves[fromSquare] & myBitboardInverted;
}

Bitboard GetKingMoves(byte fromSquare, Bitboard myBitboardInverted)
{
    return KingMoves[fromSquare] & myBitboardInverted;
}

Bitboard GetQueenMoves(byte fromSquare, Bitboard myBitboardInverted, Bitboard CompleteGamestate)
{
    Bitboard blockersRook = CompleteGamestate & RookFullBlockerMasks[fromSquare];
    Bitboard blockersBishop = CompleteGamestate & BishopFullBlockerMasks[fromSquare];
    int hashRook = TranslateRook(fromSquare, blockersRook);
    int hashBishop = TranslateBishop(fromSquare, blockersBishop);

    Bitboard moves = RookMoves[fromSquare][hashRook] | BishopMoves[fromSquare][hashBishop];
    moves &= myBitboardInverted;
    return moves;
}

Bitboard GetRookMoves(byte fromSquare, Bitboard myBitboardInverted, Bitboard CompleteGamestate)
{
    Bitboard blockers = CompleteGamestate & RookFullBlockerMasks[fromSquare];
    int hashRook = TranslateRook(fromSquare, blockers);

    Bitboard moves = RookMoves[fromSquare][hashRook] & myBitboardInverted;
    return moves;
}

Bitboard GetBishopMoves(byte fromSquare, Bitboard myBitboardInverted, Bitboard CompleteGamestate)
{
    Bitboard blockers = CompleteGamestate & BishopFullBlockerMasks[fromSquare];
    int hashBishop = TranslateBishop(fromSquare, blockers);

    Bitboard moves = BishopMoves[fromSquare][hashBishop] & myBitboardInverted;
    return moves;
}
