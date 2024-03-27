#ifndef DEFAULTS_H
#define DEFAULTS_H

#include "constants.h"

Bitboard MyPositionsStart[BITBOARD_ARRAY_SIZE], OpponentPositionsStart[BITBOARD_ARRAY_SIZE];

void initDefaults()
{
    MyPositionsStart[INDEX_PAWN_BITBOARD] = 0x000000000000FF00UL;
    MyPositionsStart[INDEX_KNIGHT_BITBOARD] = 0x0000000000000042UL;
    MyPositionsStart[INDEX_BISHOP_BITBOARD] = 0x0000000000000024UL;
    MyPositionsStart[INDEX_ROOK_BITBOARD] = 0x0000000000000081UL;
    MyPositionsStart[INDEX_QUEEN_BITBOARD] = 0x0000000000000010UL;
    MyPositionsStart[INDEX_KING_BITBOARD] = 0x0000000000000008UL;
    MyPositionsStart[INDEX_FULL_BITBOARD] = 0x000000000000FFFFUL;
    MyPositionsStart[INDEX_CASTLE_BITBOARD] = 0x0000000000000022UL;
    MyPositionsStart[INDEX_EN_PASSANT_BITBOARD] = 0UL;

    OpponentPositionsStart[INDEX_PAWN_BITBOARD] = 0x00FF000000000000UL;
    OpponentPositionsStart[INDEX_KNIGHT_BITBOARD] = 0x4200000000000000UL;
    OpponentPositionsStart[INDEX_BISHOP_BITBOARD] = 0x2400000000000000UL;
    OpponentPositionsStart[INDEX_ROOK_BITBOARD] = 0x8100000000000000UL;
    OpponentPositionsStart[INDEX_QUEEN_BITBOARD] = 0x1000000000000000UL;
    OpponentPositionsStart[INDEX_KING_BITBOARD] = 0x0800000000000000UL;
    OpponentPositionsStart[INDEX_FULL_BITBOARD] = 0xFFFF000000000000UL;
    OpponentPositionsStart[INDEX_CASTLE_BITBOARD] = 0x2200000000000000UL;
    OpponentPositionsStart[INDEX_EN_PASSANT_BITBOARD] = 0UL;
}

// incomplete
// TODO!, btw, invalid storage class
#include <stdio.h>
void LoadPositionFromFEN(string FEN)
{
    // Read into temp bitboards because we don't know which one will be myColor and which one opponentColor
    Bitboard tempWhite[BITBOARD_ARRAY_SIZE], tempBlack[BITBOARD_ARRAY_SIZE];
    int currentBoardIndex = 63; // FEN starts at A8 I think
    // Print position
    for (int i = 0; FEN[i] != '\0'; i++)
    {
        if(FEN[i] <= '9' && FEN[i] >= '0')
        {
            // Number: Skips
            currentBoardIndex -= FEN[i] - '0';
            continue;
        }

        // Pieces
        if(FEN[i] == 'P')
        {
            tempWhite[INDEX_ROOK_BITBOARD] |= 1UL << currentBoardIndex;
        }
        else if(FEN[i] == 'B')
        {
            tempWhite[INDEX_ROOK_BITBOARD] |= 1UL << currentBoardIndex;
        }
        else if(FEN[i] == 'N')
        {
            tempWhite[INDEX_ROOK_BITBOARD] |= 1UL << currentBoardIndex;
        }
        else if(FEN[i] == 'R')
        {
            tempWhite[INDEX_ROOK_BITBOARD] |= 1UL << currentBoardIndex;
        }
        else if(FEN[i] == 'Q')
        {
            tempWhite[INDEX_QUEEN_BITBOARD] |= 1UL << currentBoardIndex;
        }
        else if(FEN[i] == 'K')
        {
            tempWhite[INDEX_ROOK_BITBOARD] |= 1UL << currentBoardIndex;
        }

        else if(FEN[i] == 'p')
        {
            tempWhite[INDEX_ROOK_BITBOARD] |= 1UL << currentBoardIndex;
        }
        else if(FEN[i] == 'b')
        {
            tempWhite[INDEX_ROOK_BITBOARD] |= 1UL << currentBoardIndex;
        }
        else if(FEN[i] == 'n')
        {
            tempWhite[INDEX_ROOK_BITBOARD] |= 1UL << currentBoardIndex;
        }
        else if(FEN[i] == 'r')
        {
            tempWhite[INDEX_ROOK_BITBOARD] |= 1UL << currentBoardIndex;
        }
        else if(FEN[i] == 'q')
        {
            tempWhite[INDEX_QUEEN_BITBOARD] |= 1UL << currentBoardIndex;
        }
        else if(FEN[i] == 'k')
        {
            tempWhite[INDEX_ROOK_BITBOARD] |= 1UL << currentBoardIndex;
        }
        currentBoardIndex--;
    }
}

void printStringln(string s)
{
    // Print position
    for (int i = 0; s[i] != '\0'; i++)
    {
        printf("%c", s[i]);
    }
    printf("\n");
}

#endif