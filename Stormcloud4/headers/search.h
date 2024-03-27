#ifndef SEARCH_H
#define SEARCH_H

#include "constants.h"

static ushort Packed_Bestmove;
static int FinalSearchDepth;

extern double Search(double alpha, double beta, Bitboard* myBitboards, Bitboard* opponentBitboards,
	int depthRemaining, bool isWhite, bool isRoot);

extern double Quiesce(double alpha, double beta, Bitboard* myBitboards, Bitboard* opponentBitboards,
    bool isWhite, int QuiesceDepth);

static double BeginSearch(Bitboard* myBitboards, Bitboard* opponentBitboards, int FinalDepth, bool isWhite)
{
    Packed_Bestmove = 0;
    FinalSearchDepth = FinalDepth;
    return Search(-ALGORITHM_CONSTANT_KING_CAPTUREVALUE-1, ALGORITHM_CONSTANT_KING_CAPTUREVALUE+1, myBitboards,
        opponentBitboards, FinalDepth, isWhite, true);
}

//#include <time.h>
#include <stdio.h>
static double BeginIterativeDeepening(Bitboard* myBitboards, Bitboard* opponentBitboards, int StartDepth, int FinalDepth, bool isWhite)
{
    for(int i = StartDepth; i <= FinalDepth; i += 2)
    {
        printf("Beginning Search, Depth: %d:\n", i);
        double score = BeginSearch(myBitboards, opponentBitboards, i, isWhite);
        printf("Completed Search on Depth %d, Score: %f\n", i, score);
    }
    printf("Done.");
}

// XOR Generation Helpers

static Bitboard GetMedianBitboard(byte squareFrom, byte squareTo)
{
    return 1UL << ((squareFrom + squareTo) >> 1);
}

static byte CombineSquareData(byte squareFileData, byte squareRankData)
{
    return (byte) ((squareFileData & 0b111) | (squareRankData & 0b111000));
}

static bool DataReferencesBitboard(byte data)
{
    return !(data & 0b1000);
}

#endif