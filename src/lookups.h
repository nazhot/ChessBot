#ifndef LOOKUPS_H
#define LOOKUPS_H

#include <inttypes.h>
#include <stdlib.h>
#include "board.h"

typedef struct IndexTranslation {
    uint row;
    uint col;
    uint diaUpRight;
    uint diaUpRightIndex;
    uint diaDownRight;
    uint diaDownRightIndex;
} IndexTranslation;

void initializeLookupTables();
void lookup_setHorizontalMoves( uint64_t *moves, unsigned char rowBitMap, uint rowNumber,
                                uint colNumber );
void lookup_setVerticalMoves( uint64_t *moves, unsigned char colBitMap, uint rowNumber,
                              uint colNumber );
void lookup_setDiagonalMoves( uint64_t *moves, unsigned char diaBitMapUpRight, 
                              unsigned char diaBitMapDownRight, uint rowNumber, uint colNumber );
IndexTranslation* lookup_translateIndex( uint index );
void printMoves( uint64_t moves, uint indexToCheck, char symbol );

void getKingMoves( uint row, uint col, uint64_t *moves );
void getQueenMoves( Board *board, uint row, uint col, uint64_t *moves );
void getRookMoves( Board *board, uint row, uint col, uint64_t *moves );
void getBishopMoves( Board *board, uint row, uint col, uint64_t *moves );
void getKnightMoves( uint row, uint col, uint64_t *moves );
void getPawnMoves( Board *board, uint row, uint col, uint64_t *moves, uint64_t *captures );
#endif
