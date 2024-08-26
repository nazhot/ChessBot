#ifndef LOOKUPS_H
#define LOOKUPS_H

#include <inttypes.h>
#include <stdlib.h>

void initializeLookupTables();
void lookup_setHorizontalMoves( uint64_t *moves, char rowBitMap, uint rowNumber,
                                uint colNumber );
void lookup_setVerticalMoves( uint64_t *moves, char colBitMap, uint rowNumber,
                              uint colNumber );
void lookup_setDiagonalMoves( uint64_t *moves, unsigned char diaBitMapUpRight, 
                              unsigned char diaBitMapDownRight, uint rowNumber, uint colNumber );

#endif
