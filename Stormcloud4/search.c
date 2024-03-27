#include "headers/search.h"
#include "headers/helpers.h"
#include "headers/movegeneration.h"


// Returns score if King Captured
int GenerateXOROperations(Bitboard* myBitboards, Bitboard* opponentBitboards, ushort move,
    byte* xorOpsIndeces, Bitboard* xorOpsBitboards, byte* myCount, byte* totalCount, bool isWhite, int DepthValue)
{
    byte fromSquare = GetMoveSquareFrom(move);
    byte toSquare = GetMoveSquareTo(move);

    Bitboard unpackedFrom = 1UL << fromSquare;
    Bitboard unpackedTo = 1UL << toSquare;
    Bitboard unpacked_combined = unpackedFrom | unpackedTo;
    byte data = GetMoveData(move);

    // Todo this
    // KING CAPTURED!!! IMMEDIATE LOSS
    if ((unpackedTo & opponentBitboards[INDEX_KING_BITBOARD]) != 0)
    {

        return ALGORITHM_CONSTANT_KING_CAPTUREVALUE + DepthValue;

        // Hypothetical XOR ops: (C# variant)
        //XORBitboardOperations[OperationCount++] = (INDEX_KING_BITBOARD, unpacked.Item2);
        //XORBitboardOperations[OperationCount++] = (INDEX_FULL_BITBOARD, unpacked.Item2);
    }
    
    xorOpsIndeces[(*totalCount)] = INDEX_FULL_BITBOARD;
    xorOpsBitboards[(*totalCount)++] = unpacked_combined;

    if (DataReferencesBitboard(data))
    {
        // Remove castle options if King/Rook
        if ((unpackedFrom & myBitboards[INDEX_KING_BITBOARD]) != 0)
        {
            xorOpsIndeces[(*totalCount)] = INDEX_CASTLE_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = myBitboards[INDEX_CASTLE_BITBOARD];
        }
        else if ((unpackedFrom & myBitboards[INDEX_ROOK_BITBOARD]) != 0)
        {
            // If could castle that way before, cannot castle anymore now
            if (fromSquare == CASTLE_SQUARE_ROOK_PREV_INDEX_KINGSIDE_WHITE && CASTLE_BITMASK_CASTLE_KINGSIDE_WHITE & myBitboards[INDEX_CASTLE_BITBOARD])
            {
                xorOpsIndeces[(*totalCount)] = INDEX_CASTLE_BITBOARD;
                xorOpsBitboards[(*totalCount)++] = CASTLE_BITMASK_CASTLE_KINGSIDE_WHITE;
            }
			else if (fromSquare == CASTLE_SQUARE_ROOK_PREV_INDEX_QUEENSIDE_WHITE && CASTLE_BITMASK_CASTLE_QUEENSIDE_WHITE & myBitboards[INDEX_CASTLE_BITBOARD])
            {
                xorOpsIndeces[(*totalCount)] = INDEX_CASTLE_BITBOARD;
                xorOpsBitboards[(*totalCount)++] = CASTLE_BITMASK_CASTLE_QUEENSIDE_WHITE;
            }
			else if (fromSquare == CASTLE_SQUARE_ROOK_PREV_INDEX_KINGSIDE_BLACK && CASTLE_BITMASK_CASTLE_KINGSIDE_BLACK & myBitboards[INDEX_CASTLE_BITBOARD])
            {
                xorOpsIndeces[(*totalCount)] = INDEX_CASTLE_BITBOARD;
                xorOpsBitboards[(*totalCount)++] = CASTLE_BITMASK_CASTLE_KINGSIDE_BLACK;
            }
			else if (fromSquare == CASTLE_SQUARE_ROOK_PREV_INDEX_QUEENSIDE_BLACK && CASTLE_BITMASK_CASTLE_QUEENSIDE_BLACK & myBitboards[INDEX_CASTLE_BITBOARD])
            {
                xorOpsIndeces[(*totalCount)] = INDEX_CASTLE_BITBOARD;
                xorOpsBitboards[(*totalCount)++] = CASTLE_BITMASK_CASTLE_QUEENSIDE_BLACK;
            }
        }
        xorOpsIndeces[(*totalCount)] = data;
        xorOpsBitboards[(*totalCount)++] = unpacked_combined;
        xorOpsIndeces[(*totalCount)] = INDEX_EN_PASSANT_BITBOARD;
        xorOpsBitboards[(*totalCount)++] = myBitboards[INDEX_EN_PASSANT_BITBOARD];
        *myCount = *totalCount;
    }
	// Put this here because this does not contain a clear en passant call
	else if (data == MOVEDATA_PAWN_JUMPSTART)
	{
        xorOpsIndeces[(*totalCount)] = INDEX_EN_PASSANT_BITBOARD;
        xorOpsBitboards[(*totalCount)++] = GetMedianBitboard(fromSquare, toSquare) | myBitboards[INDEX_EN_PASSANT_BITBOARD];
		xorOpsIndeces[(*totalCount)] = INDEX_PAWN_BITBOARD;
        xorOpsBitboards[(*totalCount)++] = unpacked_combined;
		*myCount = *totalCount;
	}
	else
	{
		// Clear En Passants before putting the ball in the opponent's field, so no need to worry about opponent
		xorOpsIndeces[(*totalCount)] = INDEX_EN_PASSANT_BITBOARD;
        xorOpsBitboards[(*totalCount)++] = myBitboards[INDEX_EN_PASSANT_BITBOARD];

		if (data == MOVEDATA_CASTLE_SHORT)
		{
			int index = isWhite ? 0 : 2;

            xorOpsIndeces[(*totalCount)] = INDEX_KING_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = CASTLE_XOR_MASKS_KING[index];
            xorOpsIndeces[(*totalCount)] = INDEX_ROOK_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = CASTLE_XOR_MASKS_ROOK[index];
            // King move is added to full bitboard already, now apply the rookmove as well
            xorOpsIndeces[(*totalCount)] = INDEX_FULL_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = CASTLE_XOR_MASKS_ROOK[index];
            xorOpsIndeces[(*totalCount)] = INDEX_CASTLE_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = myBitboards[INDEX_CASTLE_BITBOARD];  // XOR with whatever we have to set to 0 and restore to OG value
            *myCount = *totalCount;
		}
		else if (data == MOVEDATA_CASTLE_LONG)
		{
			int index = isWhite ? 1 : 3;
            
            xorOpsIndeces[(*totalCount)] = INDEX_KING_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = CASTLE_XOR_MASKS_KING[index];
            xorOpsIndeces[(*totalCount)] = INDEX_ROOK_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = CASTLE_XOR_MASKS_ROOK[index];
            // King move is added to full bitboard already, now apply the rookmove as well
            xorOpsIndeces[(*totalCount)] = INDEX_FULL_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = CASTLE_XOR_MASKS_ROOK[index];
            xorOpsIndeces[(*totalCount)] = INDEX_CASTLE_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = myBitboards[INDEX_CASTLE_BITBOARD];  // XOR with whatever we have to set to 0 and restore to OG value
            *myCount = *totalCount;
		}
		else if (data == MOVEDATA_EN_PASSANT_CAPTURE)
		{
            xorOpsIndeces[(*totalCount)] = INDEX_PAWN_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = unpacked_combined;
			*myCount = *totalCount;
			// En Passant Pawn from Opponent
			byte pawnSquare = CombineSquareData(toSquare, fromSquare);
            xorOpsIndeces[(*totalCount)] = INDEX_PAWN_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = 1UL << pawnSquare;
		}
		else if (data == MOVEDATA_PROMOTION_N)
		{
            xorOpsIndeces[(*totalCount)] = INDEX_PAWN_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = unpackedFrom;
            xorOpsIndeces[(*totalCount)] = INDEX_KNIGHT_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = unpackedTo;
			*myCount = *totalCount;
		}
		else if (data == MOVEDATA_PROMOTION_B)
		{
            xorOpsIndeces[(*totalCount)] = INDEX_PAWN_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = unpackedFrom;
            xorOpsIndeces[(*totalCount)] = INDEX_BISHOP_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = unpackedTo;
			*myCount = *totalCount;
		}
		else if (data == MOVEDATA_PROMOTION_R)
		{
            xorOpsIndeces[(*totalCount)] = INDEX_PAWN_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = unpackedFrom;
            xorOpsIndeces[(*totalCount)] = INDEX_ROOK_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = unpackedTo;
			*myCount = *totalCount;
		}
		else if (data == MOVEDATA_PROMOTION_Q)
		{
            xorOpsIndeces[(*totalCount)] = INDEX_PAWN_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = unpackedFrom;
            xorOpsIndeces[(*totalCount)] = INDEX_QUEEN_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = unpackedTo;
			*myCount = *totalCount;
		}
	}

	// If RookCapture, adjust CastleOptions if necessary
	if ((unpackedTo & opponentBitboards[INDEX_ROOK_BITBOARD]) != 0)
	{
        xorOpsIndeces[(*totalCount)] = INDEX_ROOK_BITBOARD;
        xorOpsBitboards[(*totalCount)++] = unpackedTo;
        xorOpsIndeces[(*totalCount)] = INDEX_FULL_BITBOARD;
        xorOpsBitboards[(*totalCount)++] = unpackedTo;

		Bitboard bitmask = 7;  // IMPOSSIBLE VALUE (3 incorrect squares marked)
		if (toSquare == CASTLE_SQUARE_ROOK_PREV_INDEX_KINGSIDE_WHITE) bitmask = CASTLE_BITMASK_CASTLE_KINGSIDE_WHITE;
		else if (toSquare == CASTLE_SQUARE_ROOK_PREV_INDEX_QUEENSIDE_WHITE) bitmask = CASTLE_BITMASK_CASTLE_QUEENSIDE_WHITE;
		else if (toSquare == CASTLE_SQUARE_ROOK_PREV_INDEX_KINGSIDE_BLACK) bitmask = CASTLE_BITMASK_CASTLE_KINGSIDE_BLACK;
		else if (toSquare == CASTLE_SQUARE_ROOK_PREV_INDEX_QUEENSIDE_BLACK) bitmask = CASTLE_BITMASK_CASTLE_QUEENSIDE_BLACK;

		if (bitmask != 7)
        {
            // Make XORable operation so it only has effect if it's original value is 1
            xorOpsIndeces[(*totalCount)] = INDEX_CASTLE_BITBOARD;
            xorOpsBitboards[(*totalCount)++] = opponentBitboards[INDEX_CASTLE_BITBOARD] & bitmask;
        }
    }
	else if ((unpackedTo & opponentBitboards[INDEX_PAWN_BITBOARD]) != 0)
	{
        xorOpsIndeces[(*totalCount)] = INDEX_PAWN_BITBOARD;
        xorOpsBitboards[(*totalCount)++] = unpackedTo;
        xorOpsIndeces[(*totalCount)] = INDEX_FULL_BITBOARD;
        xorOpsBitboards[(*totalCount)++] = unpackedTo;
	}
	else if ((unpackedTo & opponentBitboards[INDEX_KNIGHT_BITBOARD]) != 0)
	{
        xorOpsIndeces[(*totalCount)] = INDEX_KNIGHT_BITBOARD;
        xorOpsBitboards[(*totalCount)++] = unpackedTo;
        xorOpsIndeces[(*totalCount)] = INDEX_FULL_BITBOARD;
        xorOpsBitboards[(*totalCount)++] = unpackedTo;
	}
	else if ((unpackedTo & opponentBitboards[INDEX_BISHOP_BITBOARD]) != 0)
	{
        xorOpsIndeces[(*totalCount)] = INDEX_BISHOP_BITBOARD;
        xorOpsBitboards[(*totalCount)++] = unpackedTo;
        xorOpsIndeces[(*totalCount)] = INDEX_FULL_BITBOARD;
        xorOpsBitboards[(*totalCount)++] = unpackedTo;
	}
	else if ((unpackedTo & opponentBitboards[INDEX_QUEEN_BITBOARD]) != 0)
	{
        xorOpsIndeces[(*totalCount)] = INDEX_QUEEN_BITBOARD;
        xorOpsBitboards[(*totalCount)++] = unpackedTo;
        xorOpsIndeces[(*totalCount)] = INDEX_FULL_BITBOARD;
        xorOpsBitboards[(*totalCount)++] = unpackedTo;
	}
	// King not necessary here since we check at the start

	return 0;	// score
}


void ApplyXOROperations(Bitboard* myBitboards, Bitboard* opponentBitboards, byte* indeces, Bitboard* operations, byte myCount, byte totalCount)
{
    byte c;
    for (c = 0; c < myCount; myBitboards[indeces[c]] ^= operations[c], ++c) ;
    for (; c < totalCount; opponentBitboards[indeces[c]] ^= operations[c], ++c) ;
}


double Quiescence(double alpha, double beta, Bitboard* myBitboards, Bitboard* opponentBitboards,
    bool isWhite, int QuiescenceDepth)
{
    // Basically:
    double score = -Quiescence(-beta, -alpha, opponentBitboards, myBitboards, NOT(isWhite), QuiescenceDepth+1);
    return 0;
}

double Search(double alpha, double beta, Bitboard* myBitboards, Bitboard* opponentBitboards,
	int depthRemaining, bool isWhite, bool isRoot)
{
    if(depthRemaining == 0) return 0;
    double HighestScore = NegativeInfinity;

    ushort allMoves[218];
    byte moveCount = GenerateAllMoves(myBitboards, opponentBitboards, allMoves, isWhite);

    for (int move = 0; move < moveCount; move++)
    {
        byte XOROperationsIndeces[10];
        Bitboard XOROperationsValues[10];
        
        byte myOpsCount = 0, totalOpsCount = 0;

        double score = GenerateXOROperations(myBitboards, opponentBitboards, allMoves[move], XOROperationsIndeces, XOROperationsValues, &myOpsCount, &totalOpsCount, isWhite, -FinalSearchDepth + depthRemaining);

        if(score == 0)
        {
            // Make move
            ApplyXOROperations(myBitboards, opponentBitboards, XOROperationsIndeces, XOROperationsValues, myOpsCount, totalOpsCount);

            // Get Score
            score = -Search(-beta, -alpha, opponentBitboards, myBitboards, depthRemaining-1, NOT(isWhite), false);
            
            // Unmake move
            ApplyXOROperations(myBitboards, opponentBitboards, XOROperationsIndeces, XOROperationsValues, myOpsCount, totalOpsCount);       
        }
        
        // Adjust scores
        if (score >= beta)
        {
            if (isRoot)
            {
                Packed_Bestmove = allMoves[move];
            }
            // Todo if (!double.IsInfinity(score) && score < PositiveKingCaptureEvalValue && score > NegativeKingCaptureEvalValue) CC_ForcedMate = -1;  // Remove forced mate
            // Beta Cutoff. This is a killer.
            // Todo AddKiller(move, ref position, posKey);
            return score;
        }
        if (score > HighestScore)
        {
            HighestScore = score;
            if (score > alpha)
            {
                alpha = score;
                if (isRoot)
                {
                    Packed_Bestmove = allMoves[move];
                }
            }
        }
    }

    return HighestScore;
}