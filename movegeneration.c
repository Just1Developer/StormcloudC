#include "headers/movegeneration.h"
#include "headers/helpers.h"
#include "headers/memory.h"
#include "headers/constants.h"
#include "headers/movegen.h"

#pragma region Helpers

int IsGamestateEndgame(Bitboard* myBitboards, Bitboard* opponentBitboards, Bitboard CompleteGamestate)
{
	return 0; //(myBitboards[INDEX_QUEEN_BITBOARD] & opponentBitboards[INDEX_QUEEN_BITBOARD]) == 0 && Count_1s(CompleteGamestate) < MIN_TOTAL_PIECES_ON_BOARD_FOR_MIDDLEGAME;
}

#pragma endregion

#pragma region Move Packing

#pragma region Layer 2 | All Moves

Bitboard GetAllNonPawnAttackBitboard(Bitboard* Bitboards, Bitboard completeGamestateBlockers)
{
	Bitboard possibleMoves = 0;

	// We cant really outsource this since the part from the MoveGen class is always different.

	// Sliding pieces first, Bishop:
	Bitboard AllPiecesBitboard = Bitboards[INDEX_BISHOP_BITBOARD];
	while (AllPiecesBitboard != 0)
	{
		Bitboard pieceOnlyBitboard = AllPiecesBitboard & (Bitboard)-(long)AllPiecesBitboard;
		byte square = TrailingZeroCount(pieceOnlyBitboard);

		// Partial from MoveGen class:
		Bitboard blockers = completeGamestateBlockers & BishopFullBlockerMasks[square];
		int hashBishop = TranslateBishop(square, blockers);

		possibleMoves |= BishopMoves[square][hashBishop];

		AllPiecesBitboard ^= pieceOnlyBitboard; // Clear the least significant bit set.
	}

	// Rooks:
	AllPiecesBitboard = Bitboards[INDEX_ROOK_BITBOARD];
	while (AllPiecesBitboard != 0)
	{
		Bitboard pieceOnlyBitboard = AllPiecesBitboard & (Bitboard)-(long)AllPiecesBitboard;
		byte square = TrailingZeroCount(pieceOnlyBitboard);

		// Partial from MoveGen class:
		Bitboard blockers = completeGamestateBlockers & RookFullBlockerMasks[square];
		int hashRook = TranslateRook(square, blockers);

		possibleMoves |= RookMoves[square][hashRook];

		AllPiecesBitboard ^= pieceOnlyBitboard; // Clear the least significant bit set.
	}

	// Queen(s):
	AllPiecesBitboard = Bitboards[INDEX_QUEEN_BITBOARD];
	while (AllPiecesBitboard != 0)
	{
		Bitboard pieceOnlyBitboard = AllPiecesBitboard & (Bitboard)-(long)AllPiecesBitboard;
		byte square = TrailingZeroCount(pieceOnlyBitboard);

		// Partial from MoveGen class:
		// Bishop:
		Bitboard blockers = completeGamestateBlockers & BishopFullBlockerMasks[square];
		int hashBishop = TranslateBishop(square, blockers);
		possibleMoves |= BishopMoves[square][hashBishop];
		// Rook:
		blockers = completeGamestateBlockers & RookFullBlockerMasks[square];
		hashBishop = TranslateRook(square, blockers);
		possibleMoves |= RookMoves[square][hashBishop];

		AllPiecesBitboard ^= pieceOnlyBitboard; // Clear the least significant bit set.
	}

	// Knights:
	AllPiecesBitboard = Bitboards[INDEX_KNIGHT_BITBOARD];
	while (AllPiecesBitboard != 0)
	{
		Bitboard pieceOnlyBitboard = AllPiecesBitboard & (Bitboard)-(long)AllPiecesBitboard;
		int square = TrailingZeroCount(pieceOnlyBitboard);

		// Partial from MoveGen class:
		possibleMoves |= KnightMoves[square];

		AllPiecesBitboard ^= pieceOnlyBitboard; // Clear the least significant bit set.
	}

	// King:
	AllPiecesBitboard = Bitboards[INDEX_KING_BITBOARD];
	// Only 1 king, 100%
	{
		Bitboard pieceOnlyBitboard = AllPiecesBitboard & (Bitboard)-(long)AllPiecesBitboard;
		byte square = TrailingZeroCount(pieceOnlyBitboard);

		// Partial from MoveGen class:
		possibleMoves |= KingMoves[square];
	}

	return possibleMoves;
}

void ApplyAllPawnAttackBitboardWhite(Bitboard pawnBitboard, Bitboard* moveBitboard)
{
	*moveBitboard |= (pawnBitboard & Bitboard_NotAFile) << 9;	// West
	*moveBitboard |= (pawnBitboard & Bitboard_NotHFile) << 7;	// East
}

void ApplyAllPawnAttackBitboardBlack(Bitboard pawnBitboard, Bitboard* moveBitboard)
{
	*moveBitboard |= (pawnBitboard & Bitboard_NotAFile) >> 7;	// West
	*moveBitboard |= (pawnBitboard & Bitboard_NotHFile) >> 9;	// East
}

void MoveGen_PackMovesFromBitboard(Bitboard LegalMoveBitboard, ushort* moves, byte fromSquare, byte* move, byte movedata)
{
	// Trust inversion has happened when move generation
	// From GPT-4:
	while (LegalMoveBitboard != 0)
	{
		Bitboard toMove = LegalMoveBitboard & (Bitboard)-(long)LegalMoveBitboard;
		byte toSquare = TrailingZeroCount(toMove);

		moves[(*move)++] = Pack(fromSquare, toSquare, movedata);

		LegalMoveBitboard ^= toMove; // Clear the least significant bit set.
	}
}

// Also perhaps disallow moves where the King walks into a check.
// We could also calculate pins for pinned pieces, but that is too much computation for here.
// But on the other hand, we get attacked squares anyway, and instead of just ruling out castleing, why not rule out kingmoves too.
// => Just also NAND it with the King move bitboard
void PackKingMovesAndCastleOptions(Bitboard* myBitboards, Bitboard* opponentBitboards, Bitboard combinedBitboard,
	Bitboard myBitboardInverted, ushort* moves, byte fromSquare, byte* move, int isWhite)
{
	// Generate All Attacks of Opponent
	Bitboard opponentAttacks = GetAllNonPawnAttackBitboard(opponentBitboards, combinedBitboard);
	if(isWhite) ApplyAllPawnAttackBitboardBlack(opponentBitboards[INDEX_PAWN_BITBOARD], &opponentAttacks);
	else ApplyAllPawnAttackBitboardWhite(opponentBitboards[INDEX_PAWN_BITBOARD], &opponentAttacks);

	Bitboard KingMoves = GetKingMoves(fromSquare, myBitboardInverted);
	MoveGen_PackMovesFromBitboard(KingMoves & ~opponentAttacks, moves, fromSquare, move, INDEX_KING_BITBOARD);

	// All moves that would be castle castle moves (allowed)
	// Lets temp them out
	Bitboard castleOptions = myBitboards[INDEX_CASTLE_BITBOARD];

	if (isWhite)
	{
		Bitboard shortCastle = CASTLE_SQUAREMASK_VULNERABLE_KINGSIDE_WHITE & castleOptions & CASTLE_SQUARES_MUST_BE_FREE_KINGSIDE_WHITE & myBitboardInverted;
		if (shortCastle != 0) if ((shortCastle & opponentAttacks) == 0)
			moves[(*move)++] = Pack(fromSquare, CASTLE_TO_SQUARE_KING_INDEX_KINGSIDE_WHITE, MOVEDATA_CASTLE_SHORT);
		Bitboard longCastle = CASTLE_SQUAREMASK_VULNERABLE_QUEENSIDE_WHITE & castleOptions & CASTLE_SQUARES_MUST_BE_FREE_QUEENSIDE_WHITE & myBitboardInverted;
		if (longCastle != 0) if ((longCastle & opponentAttacks) == 0)
			moves[(*move)++] = Pack(fromSquare, CASTLE_TO_SQUARE_KING_INDEX_QUEENSIDE_WHITE, MOVEDATA_CASTLE_LONG);
	}
	else
	{
		Bitboard shortCastle = CASTLE_SQUAREMASK_VULNERABLE_KINGSIDE_BLACK & castleOptions & CASTLE_SQUARES_MUST_BE_FREE_KINGSIDE_BLACK & myBitboardInverted;
		if(shortCastle != 0) if ((shortCastle & opponentAttacks) == 0)
			moves[(*move)++] = Pack(fromSquare, CASTLE_TO_SQUARE_KING_INDEX_KINGSIDE_BLACK, MOVEDATA_CASTLE_SHORT);
		Bitboard longCastle = CASTLE_SQUAREMASK_VULNERABLE_QUEENSIDE_BLACK & castleOptions & CASTLE_SQUARES_MUST_BE_FREE_QUEENSIDE_BLACK & myBitboardInverted;
		if(longCastle != 0) if ((longCastle & opponentAttacks) == 0)
			moves[(*move)++] = Pack(fromSquare, CASTLE_TO_SQUARE_KING_INDEX_QUEENSIDE_BLACK, MOVEDATA_CASTLE_LONG);
	}
}

void GenerateAllPawnMovesWhite_CapturesOnly(Bitboard* opponentBitboards, ushort* moves, byte square, byte* move);
void GenerateAllPawnMovesBlack_CapturesOnly(Bitboard* opponentBitboards, ushort* moves, byte square, byte* move);

void GenerateAllPawnMovesWhite(Bitboard* opponentBitboards, Bitboard CompleteGamestate, ushort* moves, byte square, byte* move)
{
	byte squareShift3 = (byte)(square >> 3);
	if (((CompleteGamestate >> (square + 8)) & 1) == 0)
	{
		if (squareShift3 != 6)
		{
			moves[(*move)++] = Pack(square, square + 8, INDEX_PAWN_BITBOARD);
			if (squareShift3 == 1 && ((CompleteGamestate >> (square + 16)) & 1) == 0)
				moves[(*move)++] = Pack(square, square + 16, MOVEDATA_PAWN_JUMPSTART);
		}
		else
		{
			moves[(*move)++] = Pack(square, square + 8, MOVEDATA_PROMOTION_Q);
			moves[(*move)++] = Pack(square, square + 8, MOVEDATA_PROMOTION_R);
			moves[(*move)++] = Pack(square, square + 8, MOVEDATA_PROMOTION_B);
			moves[(*move)++] = Pack(square, square + 8, MOVEDATA_PROMOTION_N);
		}
	}

	GenerateAllPawnMovesWhite_CapturesOnly(opponentBitboards, moves, square, move);
}

void GenerateAllPawnMovesBlack(Bitboard* opponentBitboards, Bitboard CompleteGamestate, ushort* moves, byte square, byte* move)
{
	byte squareShift3 = (byte)(square >> 3);
	if (((CompleteGamestate >> (square - 8)) & 1) == 0)
	{
		if (squareShift3 != 1)
		{
			moves[(*move)++] = Pack(square, square - 8, INDEX_PAWN_BITBOARD);
			if (squareShift3 == 6 && ((CompleteGamestate >> (square - 16)) & 1) == 0)
				moves[(*move)++] = Pack(square, square - 16, MOVEDATA_PAWN_JUMPSTART);
		}
		else
		{
			moves[(*move)++] = Pack(square, square - 8, MOVEDATA_PROMOTION_Q);
			moves[(*move)++] = Pack(square, square - 8, MOVEDATA_PROMOTION_R);
			moves[(*move)++] = Pack(square, square - 8, MOVEDATA_PROMOTION_B);
			moves[(*move)++] = Pack(square, square - 8, MOVEDATA_PROMOTION_N);
		}
	}

	GenerateAllPawnMovesBlack_CapturesOnly(opponentBitboards, moves, square, move);
}

#pragma endregion

#pragma region Layer 1 | All Moves

void PackQueenMoves(Bitboard* CurrentPieces, Bitboard* myBitboards, Bitboard myBitboardInverted, Bitboard CompleteGamestate, ushort* moves, byte* move)
{
	*CurrentPieces = myBitboards[INDEX_QUEEN_BITBOARD];
	while (*CurrentPieces != 0)
	{
		Bitboard moveBitboard = *CurrentPieces & (Bitboard)-(long)*CurrentPieces;
		byte fromSquare = TrailingZeroCount(moveBitboard);

		Bitboard QueenMoves = GetQueenMoves(fromSquare, myBitboardInverted, CompleteGamestate);
		MoveGen_PackMovesFromBitboard(QueenMoves, moves, fromSquare, move, INDEX_QUEEN_BITBOARD);

		*CurrentPieces ^= moveBitboard;
	}
}

void PackRookMoves(Bitboard* CurrentPieces, Bitboard* myBitboards, Bitboard myBitboardInverted, Bitboard CompleteGamestate, ushort* moves, byte* move)
{
	*CurrentPieces = myBitboards[INDEX_ROOK_BITBOARD];
	while (*CurrentPieces != 0)
	{
		Bitboard moveBitboard = *CurrentPieces & (Bitboard)-(long)*CurrentPieces;
		byte fromSquare = TrailingZeroCount(moveBitboard);

		Bitboard RookMoves = GetRookMoves(fromSquare, myBitboardInverted, CompleteGamestate);
		MoveGen_PackMovesFromBitboard(RookMoves, moves, fromSquare, move, INDEX_ROOK_BITBOARD);

		*CurrentPieces ^= moveBitboard;
	}
}

void PackBishopMoves(Bitboard* CurrentPieces, Bitboard* myBitboards, Bitboard myBitboardInverted, Bitboard CompleteGamestate, ushort* moves, byte* move)
{
	*CurrentPieces = myBitboards[INDEX_BISHOP_BITBOARD];
	while (*CurrentPieces != 0)
	{
		Bitboard moveBitboard = *CurrentPieces & (Bitboard)-(long)*CurrentPieces;
		byte fromSquare = TrailingZeroCount(moveBitboard);

		Bitboard BishopMoves = GetBishopMoves(fromSquare, myBitboardInverted, CompleteGamestate);
		MoveGen_PackMovesFromBitboard(BishopMoves, moves, fromSquare, move, INDEX_BISHOP_BITBOARD);

		*CurrentPieces ^= moveBitboard;
	}
}

void PackKnightMoves(Bitboard* CurrentPieces, Bitboard* myBitboards, Bitboard myBitboardInverted, ushort* moves, byte* move)
{
	*CurrentPieces = myBitboards[INDEX_KNIGHT_BITBOARD];
	while (*CurrentPieces != 0)
	{
		Bitboard moveBitboard = *CurrentPieces & (Bitboard)-(long)*CurrentPieces;
		byte fromSquare = TrailingZeroCount(moveBitboard);

		Bitboard KnightMoves = GetKnightMoves(fromSquare, myBitboardInverted);
		MoveGen_PackMovesFromBitboard(KnightMoves, moves, fromSquare, move, INDEX_KNIGHT_BITBOARD);

		*CurrentPieces ^= moveBitboard;
	}
}

void PackPawnMoves(Bitboard* CurrentPieces, Bitboard* myBitboards, Bitboard* opponentBitboards, Bitboard CompleteGamestate, ushort* moves, byte* move, int isWhite)
{
	*CurrentPieces = myBitboards[INDEX_PAWN_BITBOARD];
	while (*CurrentPieces != 0)
	{
		Bitboard moveBitboard = *CurrentPieces & (Bitboard)-(long)*CurrentPieces;
		byte fromSquare = TrailingZeroCount(moveBitboard);

		if (isWhite)
			GenerateAllPawnMovesWhite(opponentBitboards, CompleteGamestate, moves, fromSquare, move);
		else
			GenerateAllPawnMovesBlack(opponentBitboards, CompleteGamestate, moves, fromSquare, move);

		*CurrentPieces ^= moveBitboard;
	}
}

void PackKingMoves(Bitboard* CurrentPieces, Bitboard* myBitboards, Bitboard* opponentBitboards, Bitboard myBitboardInverted, Bitboard CompleteGamestate, ushort* moves, byte* move, int isWhite)
{
	*CurrentPieces = myBitboards[INDEX_KING_BITBOARD];
	if ((*CurrentPieces) == 0) return;
	*CurrentPieces = myBitboards[INDEX_KING_BITBOARD];
	Bitboard moveBitboard = *CurrentPieces & (Bitboard)-(long)*CurrentPieces;
	byte fromSquare = TrailingZeroCount(moveBitboard);

	PackKingMovesAndCastleOptions(myBitboards, opponentBitboards, CompleteGamestate, myBitboardInverted,
		moves, fromSquare, move, isWhite);
}

#pragma endregion

#pragma region Layer 2 | Captures Only

void GenerateAllPawnMovesWhite_CapturesOnly(Bitboard* opponentBitboards, ushort* moves, byte square, byte* move)
{
	byte squareShift3 = (byte)(square >> 3);

	// West Attack, if (NOT A-File AND opponent has piece on there)
	if ((square & 0b111) != 7)
	{
		if (((opponentBitboards[INDEX_FULL_BITBOARD] >> (square + 9)) & 1) == 1)
		{
			if (squareShift3 != 6) moves[(*move)++] = Pack(square, square + 9, INDEX_PAWN_BITBOARD);
			else
			{
				moves[(*move)++] = Pack(square, square + 9, MOVEDATA_PROMOTION_Q);	// Is first move here so Auto-Queen, but remember to put something in the UI choose
				moves[(*move)++] = Pack(square, square + 9, MOVEDATA_PROMOTION_R);
				moves[(*move)++] = Pack(square, square + 9, MOVEDATA_PROMOTION_B);
				moves[(*move)++] = Pack(square, square + 9, MOVEDATA_PROMOTION_N);
			}
		}
		else if (((opponentBitboards[INDEX_EN_PASSANT_BITBOARD] >> (square + 9)) & 1) == 1)
		{
			// En passant is never a promotion
			moves[(*move)++] = Pack(square, square + 9, MOVEDATA_EN_PASSANT_CAPTURE);
		}
	}

	// East Attack, if (NOT H-File AND opponent has piece on there)
	if ((square & 0b111) != 0)
	{
		if (((opponentBitboards[INDEX_FULL_BITBOARD] >> (square + 7)) & 1) == 1)
		{
			if (squareShift3 != 6) moves[(*move)++] = Pack(square, square + 7, INDEX_PAWN_BITBOARD);
			else
			{
				moves[(*move)++] = Pack(square, square + 7, MOVEDATA_PROMOTION_Q);
				moves[(*move)++] = Pack(square, square + 7, MOVEDATA_PROMOTION_R);
				moves[(*move)++] = Pack(square, square + 7, MOVEDATA_PROMOTION_B);
				moves[(*move)++] = Pack(square, square + 7, MOVEDATA_PROMOTION_N);
			}
		}
		else if (((opponentBitboards[INDEX_EN_PASSANT_BITBOARD] >> (square + 7)) & 1) == 1)
		{
			// En passant is never a promotion
			moves[(*move)++] = Pack(square, square + 7, MOVEDATA_EN_PASSANT_CAPTURE);
		}
	}
}

void GenerateAllPawnMovesBlack_CapturesOnly(Bitboard* opponentBitboards, ushort* moves, byte square, byte* move)
{
	byte squareShift3 = (byte)(square >> 3);
	// West Attack, if (NOT A-File AND opponent has piece on there)
	if ((square & 0b111) != 7)
	{
		if (((opponentBitboards[INDEX_FULL_BITBOARD] >> (square - 7)) & 1) == 1)
		{
			if (squareShift3 != 1) moves[(*move)++] = Pack(square, square - 7, INDEX_PAWN_BITBOARD);
			else
			{
				moves[(*move)++] = Pack(square, square - 7, MOVEDATA_PROMOTION_Q);
				moves[(*move)++] = Pack(square, square - 7, MOVEDATA_PROMOTION_R);
				moves[(*move)++] = Pack(square, square - 7, MOVEDATA_PROMOTION_B);
				moves[(*move)++] = Pack(square, square - 7, MOVEDATA_PROMOTION_N);
			}
		}
		else if (((opponentBitboards[INDEX_EN_PASSANT_BITBOARD] >> (square - 7)) & 1) == 1)
		{
			// En passant is never a promotion
			moves[(*move)++] = Pack(square, square - 7, MOVEDATA_EN_PASSANT_CAPTURE);
		}
	}

	// East Attack, if (NOT H-File AND opponent has piece on there)
	if ((square & 0b111) != 0)
	{
		if (((opponentBitboards[INDEX_FULL_BITBOARD] >> (square - 9)) & 1) == 1)
		{
			if (squareShift3 != 1) moves[(*move)++] = Pack(square, square - 9, INDEX_PAWN_BITBOARD);
			else
			{
				moves[(*move)++] = Pack(square, square - 9, MOVEDATA_PROMOTION_Q);
				moves[(*move)++] = Pack(square, square - 9, MOVEDATA_PROMOTION_R);
				moves[(*move)++] = Pack(square, square - 9, MOVEDATA_PROMOTION_B);
				moves[(*move)++] = Pack(square, square - 9, MOVEDATA_PROMOTION_N);
			}
		}
		else if (((opponentBitboards[INDEX_EN_PASSANT_BITBOARD] >> (square - 9)) & 1) == 1)
		{
			// En passant is never a promotion
			moves[(*move)++] = Pack(square, square - 9, MOVEDATA_EN_PASSANT_CAPTURE);
		}
	}
}

#pragma endregion

#pragma region Layer 1 | Captures Only

void PackQueenMoves_CapturesOnly(Bitboard* CurrentPieces, Bitboard* myBitboards, Bitboard* opponentBitboards, Bitboard myBitboardInverted, Bitboard CompleteGamestate, ushort* moves, byte* move)
{
	*CurrentPieces = myBitboards[INDEX_QUEEN_BITBOARD];
	while (*CurrentPieces != 0)
	{
		Bitboard moveBitboard = *CurrentPieces & (Bitboard)-(long)*CurrentPieces;
		byte fromSquare = TrailingZeroCount(moveBitboard);

		Bitboard QueenMoves = GetQueenMoves(fromSquare, myBitboardInverted, CompleteGamestate);
		MoveGen_PackMovesFromBitboard(QueenMoves & opponentBitboards[INDEX_FULL_BITBOARD], moves, fromSquare, move, INDEX_QUEEN_BITBOARD);

		*CurrentPieces ^= moveBitboard;
	}
}

void PackRookMoves_CapturesOnly(Bitboard* CurrentPieces, Bitboard* myBitboards, Bitboard* opponentBitboards, Bitboard myBitboardInverted, Bitboard CompleteGamestate, ushort* moves, byte* move)
{
	*CurrentPieces = myBitboards[INDEX_ROOK_BITBOARD];
	while (*CurrentPieces != 0)
	{
		Bitboard moveBitboard = *CurrentPieces & (Bitboard)-(long)*CurrentPieces;
		byte fromSquare = TrailingZeroCount(moveBitboard);

		Bitboard RookMoves = GetRookMoves(fromSquare, myBitboardInverted, CompleteGamestate);
		MoveGen_PackMovesFromBitboard(RookMoves & opponentBitboards[INDEX_FULL_BITBOARD], moves, fromSquare, move, INDEX_ROOK_BITBOARD);

		*CurrentPieces ^= moveBitboard;
	}
}

void PackBishopMoves_CapturesOnly(Bitboard* CurrentPieces, Bitboard* myBitboards, Bitboard* opponentBitboards, Bitboard myBitboardInverted, Bitboard CompleteGamestate, ushort* moves, byte* move)
{
	*CurrentPieces = myBitboards[INDEX_BISHOP_BITBOARD];
	while (*CurrentPieces != 0)
	{
		Bitboard moveBitboard = *CurrentPieces & (Bitboard)-(long)*CurrentPieces;
		byte fromSquare = TrailingZeroCount(moveBitboard);

		Bitboard BishopMoves = GetBishopMoves(fromSquare, myBitboardInverted, CompleteGamestate);
		MoveGen_PackMovesFromBitboard(BishopMoves & opponentBitboards[INDEX_FULL_BITBOARD], moves, fromSquare, move, INDEX_BISHOP_BITBOARD);

		*CurrentPieces ^= moveBitboard;
	}
}

void PackKnightMoves_CapturesOnly(Bitboard* CurrentPieces, Bitboard* myBitboards, Bitboard* opponentBitboards, Bitboard myBitboardInverted, ushort* moves, byte* move)
{
	*CurrentPieces = myBitboards[INDEX_KNIGHT_BITBOARD];
	while (*CurrentPieces != 0)
	{
		Bitboard moveBitboard = *CurrentPieces & (Bitboard)-(long)*CurrentPieces;
		byte fromSquare = TrailingZeroCount(moveBitboard);

		Bitboard KnightMoves = GetKnightMoves(fromSquare, myBitboardInverted);
		MoveGen_PackMovesFromBitboard(KnightMoves & opponentBitboards[INDEX_FULL_BITBOARD], moves, fromSquare, move, INDEX_KING_BITBOARD);

		*CurrentPieces ^= moveBitboard;
	}
}

void PackPawnMoves_CapturesOnly(Bitboard* CurrentPieces, Bitboard* myBitboards, Bitboard* opponentBitboards, ushort* moves, byte* move, int isWhite)
{
	*CurrentPieces = myBitboards[INDEX_PAWN_BITBOARD];
	while (*CurrentPieces != 0)
	{
		Bitboard moveBitboard = *CurrentPieces & (Bitboard)-(long)*CurrentPieces;
		byte fromSquare = TrailingZeroCount(moveBitboard);

		if (isWhite)
			GenerateAllPawnMovesWhite_CapturesOnly(opponentBitboards, moves, fromSquare, move);
		else
			GenerateAllPawnMovesBlack_CapturesOnly(opponentBitboards, moves, fromSquare, move);

		*CurrentPieces ^= moveBitboard;
	}
}

void PackKingMoves_CapturesOnly(Bitboard* CurrentPieces, Bitboard* myBitboards, Bitboard* opponentBitboards, Bitboard myBitboardInverted, Bitboard CompleteGamestate, ushort* moves, byte* move, int isWhite)
{
	if (*CurrentPieces == 0) return;
	*CurrentPieces = myBitboards[INDEX_KING_BITBOARD];
	Bitboard moveBitboard = *CurrentPieces & (Bitboard)-(long)*CurrentPieces;
	byte fromSquare = TrailingZeroCount(moveBitboard);

	// Generate All Attacks of Opponent
	Bitboard opponentAttacks = GetAllNonPawnAttackBitboard(opponentBitboards, CompleteGamestate);
	if (isWhite) ApplyAllPawnAttackBitboardBlack(opponentBitboards[INDEX_PAWN_BITBOARD], &opponentAttacks);	// Opposite color because we are calculating opponent's moves
	else ApplyAllPawnAttackBitboardWhite(opponentBitboards[INDEX_PAWN_BITBOARD], &opponentAttacks);

	Bitboard KingMoves = GetKingMoves(fromSquare, myBitboardInverted);
	MoveGen_PackMovesFromBitboard(KingMoves & ~opponentAttacks & opponentBitboards[INDEX_FULL_BITBOARD], moves, fromSquare, move, INDEX_KING_BITBOARD);

	// Castle will never be a capture, so we can leave it out
}


#pragma endregion

#pragma endregion

#pragma region Capture Only Move Generation

/// This region servers for capture only move generations
/// byte GenerateAllMoves_CapturesOnly(Bitboard* myBitboards, Bitboard* opponentBitboards, ushort* moves, int isWhite)
/// => GenerateAllMoves_CapturesOnly(myBitboards, opponentBitboards, myBitboards[INDEX_FULL_BITBOARD] | opponentBitboards[INDEX_FULL_BITBOARD], moves, isWhite);
byte GenerateAllMoves_CapturesOnly(Bitboard* myBitboards, Bitboard* opponentBitboards, ushort* moves, bool isWhite)
{
	Bitboard CompleteGamestate = myBitboards[INDEX_FULL_BITBOARD] | opponentBitboards[INDEX_FULL_BITBOARD];
	Bitboard myBitboardInverted = ~myBitboards[INDEX_FULL_BITBOARD];
	byte move = 0;

	// differentiating order in endgame

	// Very simple right now: Neither side has a queen and total pieces on the board (og: 20) are < 20 (or constant, 20 is first constant value, might change)
	int IsEndgame = IsGamestateEndgame(myBitboards, opponentBitboards, CompleteGamestate);


	Bitboard CurrentPieces;
	if (IsEndgame)
	{
		PackQueenMoves_CapturesOnly(&CurrentPieces, myBitboards, opponentBitboards, myBitboardInverted, CompleteGamestate, moves, &move);
		PackRookMoves_CapturesOnly(&CurrentPieces, myBitboards, opponentBitboards, myBitboardInverted, CompleteGamestate, moves, &move);
		PackBishopMoves_CapturesOnly(&CurrentPieces, myBitboards, opponentBitboards, myBitboardInverted, CompleteGamestate, moves, &move);
		PackKnightMoves_CapturesOnly(&CurrentPieces, myBitboards, opponentBitboards, myBitboardInverted, moves, &move);
		PackPawnMoves_CapturesOnly(&CurrentPieces, myBitboards, opponentBitboards, moves, &move, isWhite);
		PackKingMoves_CapturesOnly(&CurrentPieces, myBitboards, opponentBitboards, myBitboardInverted, CompleteGamestate,
			moves, &move, isWhite);
	}
	else
	{
		PackPawnMoves_CapturesOnly(&CurrentPieces, myBitboards, opponentBitboards, moves, &move, isWhite);
		PackKnightMoves_CapturesOnly(&CurrentPieces, myBitboards, opponentBitboards, myBitboardInverted, moves, &move);
		PackBishopMoves_CapturesOnly(&CurrentPieces, myBitboards, opponentBitboards, myBitboardInverted, CompleteGamestate, moves, &move);
		PackQueenMoves_CapturesOnly(&CurrentPieces, myBitboards, opponentBitboards, myBitboardInverted, CompleteGamestate, moves, &move);
		PackRookMoves_CapturesOnly(&CurrentPieces, myBitboards, opponentBitboards, myBitboardInverted, CompleteGamestate, moves, &move);
		PackKingMoves_CapturesOnly(&CurrentPieces, myBitboards, opponentBitboards, myBitboardInverted, CompleteGamestate,
			moves, &move, isWhite);
	}

	return move;
}

#pragma endregion

#pragma region Default Move Generation

///byte GenerateAllMoves(Bitboard* myBitboards, Bitboard* opponentBitboards, ushort* moves, int isWhite)
///{
///	return GenerateAllMoves(myBitboards, opponentBitboards, myBitboards[INDEX_FULL_BITBOARD] | opponentBitboards[INDEX_FULL_BITBOARD], moves, isWhite);
///}
byte GenerateAllMoves(Bitboard* myBitboards, Bitboard* opponentBitboards, ushort* moves, bool isWhite)
{
	Bitboard CompleteGamestate = myBitboards[INDEX_FULL_BITBOARD] | opponentBitboards[INDEX_FULL_BITBOARD];
	Bitboard myBitboardInverted = ~myBitboards[INDEX_FULL_BITBOARD];
	byte move = 0;
	
	// differentiating order in endgame

	// Very simple right now: Neither side has a queen and total pieces on the board (og: 20) are < 20 (or constant, 20 is first constant value, might change)
	int IsEndgame = IsGamestateEndgame(myBitboards, opponentBitboards, CompleteGamestate);


	Bitboard CurrentPieces;
	if (IsEndgame)
	{
		PackQueenMoves(&CurrentPieces, myBitboards, myBitboardInverted, CompleteGamestate, moves, &move);
		PackRookMoves(&CurrentPieces, myBitboards, myBitboardInverted, CompleteGamestate, moves, &move);
		PackBishopMoves(&CurrentPieces, myBitboards, myBitboardInverted, CompleteGamestate, moves, &move);
		PackKnightMoves(&CurrentPieces, myBitboards, myBitboardInverted, moves, &move);
		PackPawnMoves(&CurrentPieces, myBitboards, opponentBitboards, CompleteGamestate, moves, &move, isWhite);
		PackKingMoves(&CurrentPieces, myBitboards, opponentBitboards, myBitboardInverted, CompleteGamestate,
			moves, &move, isWhite);
	}
	else
	{
		PackKnightMoves(&CurrentPieces, myBitboards, myBitboardInverted, moves, &move);
		PackPawnMoves(&CurrentPieces, myBitboards, opponentBitboards, CompleteGamestate, moves, &move, isWhite);
		PackBishopMoves(&CurrentPieces, myBitboards, myBitboardInverted, CompleteGamestate, moves, &move);
		PackQueenMoves(&CurrentPieces, myBitboards, myBitboardInverted, CompleteGamestate, moves, &move);
		PackRookMoves(&CurrentPieces, myBitboards, myBitboardInverted, CompleteGamestate, moves, &move);
		PackKingMoves(&CurrentPieces, myBitboards, opponentBitboards, myBitboardInverted, CompleteGamestate,
			moves, &move, isWhite);
	}

	return move;
}

#pragma endregion