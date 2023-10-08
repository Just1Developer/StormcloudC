#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#define BOARD_SIZE 64
#define BITMASK_SQUARE 0b111111
#define BITMASK_MOVEDATA 0b1111
#define BITMASK_FILE 0b111
#define BITMASK_RANK 0b111000

/*
#define MAX_FILE_VALUE 7
#define MAX_RANK_VALUE 7

#define GET_RANK_OF_SQUARE (x) (x >> 3)
#define GET_RANK_OF_SQUARE (x) (x & BITMASK_FILE)
*/

typedef unsigned long long Bitboard;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef signed char sbyte;

#endif