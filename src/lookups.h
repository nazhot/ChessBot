#ifndef LOOKUPS_H
#define LOOKUPS_H

#include <inttypes.h>
#include <stdlib.h>
#include "board.h"

void initializeLookupTables();
IndexTranslation* lookup_translateIndex( uint index );
uint64_t lookup_getKingMoves( const uint row, const uint col );
uint64_t lookup_getQueenMoves( const Board* const board, const uint row, const uint col );
uint64_t lookup_getRookMoves( const Board* const board, const uint row, const uint col );
uint64_t lookup_getBishopMoves( const Board* const board, const uint row, const uint col );
uint64_t lookup_getKnightMoves( const uint row, const uint col );
uint64_t lookup_getPawnMoves( const Board* const board, const uint row, const uint col );
uint64_t lookup_getPawnCaptures( const uint index, const bool isWhite );
#endif
