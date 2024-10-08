#ifndef LOOKUPS_H
#define LOOKUPS_H

#include <inttypes.h>
#include <stdlib.h>
#include "board.h"

void initializeLookupTables();
void lookup_setHorizontalMoves( uint64_t *moves, unsigned char rowBitMap, uint rowNumber,
                                uint colNumber );
void lookup_setVerticalMoves( uint64_t *moves, unsigned char colBitMap, uint rowNumber,
                              uint colNumber );
void lookup_setDiagonalMoves( uint64_t *moves, unsigned char diaBitMapUpRight, 
                              unsigned char diaBitMapDownRight, uint rowNumber, uint colNumber );
IndexTranslation* lookup_translateIndex( uint index );
void printMoves( uint64_t moves, uint indexToCheck, char symbol );

uint64_t getKingMoves( uint row, uint col );
uint64_t getQueenMoves( Board *board, uint row, uint col );
uint64_t getRookMoves( Board *board, uint row, uint col );
uint64_t getBishopMoves( Board *board, uint row, uint col );
uint64_t getKnightMoves( uint row, uint col );
uint64_t getPawnMoves( Board *board, uint row, uint col );
#endif
