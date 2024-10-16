#include "board.h"
#include "lookups.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

uint64_t kingLookupTable[64] = {0};
uint64_t knightLookupTable[64] = {0};
char hvdLookupTable[256][8] = {0}; //first index: binary representation of row. second index: position of piece being checked
const uint rcToUpRight[8][8] = { { 0, 1, 2, 3, 4, 5, 6, 7 },
                           { 1, 2, 3, 4, 5, 6, 7, 8 },
                           { 2, 3, 4, 5, 6, 7, 8, 9 },
                           { 3, 4, 5, 6, 7, 8, 9, 10 },
                           { 4, 5, 6, 7, 8, 9, 10, 11 },
                           { 5, 6, 7, 8, 9, 10, 11, 12 },
                           { 6, 7, 8, 9, 10, 11, 12, 13 },
                           { 7, 8, 9, 10, 11, 12, 13, 14 } };

const uint rcToDownRight[8][8] = { { 7, 8, 9, 10, 11, 12, 13, 14 },
                              { 6, 7, 8, 9, 10, 11, 12, 13 },
                              { 5, 6, 7, 8, 9, 10, 11, 12 },
                              { 4, 5, 6, 7, 8, 9, 10, 11 },
                              { 3, 4, 5, 6, 7, 8, 9, 10 },
                              { 2, 3, 4, 5, 6, 7, 8, 9 },
                              { 1, 2, 3, 4, 5, 6, 7, 8 },
                              { 0, 1, 2, 3, 4, 5, 6, 7 } };

const uint rcToUpRightIndex[8][8] = { { 0, 1, 2, 3, 4, 5, 6, 7 },
                                { 0, 1, 2, 3, 4, 5, 6, 6 },
                                { 0, 1, 2, 3, 4, 5, 5, 5 },
                                { 0, 1, 2, 3, 4, 4, 4, 4 },
                                { 0, 1, 2, 3, 3, 3, 3, 3 },
                                { 0, 1, 2, 2, 2, 2, 2, 2 },
                                { 0, 1, 1, 1, 1, 1, 1, 1 },
                                { 0, 0, 0, 0, 0, 0, 0, 0 } };

const uint rcToDownRightIndex[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 },
                                  { 0, 1, 1, 1, 1, 1, 1, 1 },
                                  { 0, 1, 2, 2, 2, 2, 2, 2 },
                                  { 0, 1, 2, 3, 3, 3, 3, 3 },
                                  { 0, 1, 2, 3, 4, 4, 4, 4 },
                                  { 0, 1, 2, 3, 4, 5, 5, 5 },
                                  { 0, 1, 2, 3, 4, 5, 6, 6 },
                                  { 0, 1, 2, 3, 4, 5, 6, 7 } };

uint64_t pawnCaptureLookupTable[64][2] = {0};

IndexTranslation indexTranslations[64] = {0};

static void addMove( const int row, const int col, uint64_t* const moves ) {
    if ( row >= 0 && row <= 7 && col >= 0 && col <= 7 ) {
        uint64_t temp = 1;
        *moves |= ( temp << ( 63 - ( row * 8 + col ) ) );
    }
}

static void initializeIndexTranslations() {
    for ( uint i = 0; i < 64; ++i  ) {
        uint row = i / 8;
        uint col = i % 8;
        uint rowDiff = 7 - row;
        uint colDiff = col;
        uint diaUpRightIndex = rowDiff < colDiff ? rowDiff : colDiff;
        uint diaDownRightIndex = row < col ? row : col;

        indexTranslations[i].index = i;
        indexTranslations[i].row = row;
        indexTranslations[i].col = col;
        indexTranslations[i].diaUpRight = rcToUpRight[row][col];
        indexTranslations[i].diaUpRightIndex = diaUpRightIndex;
        indexTranslations[i].diaDownRight = rcToDownRight[row][col];
        indexTranslations[i].diaDownRightIndex = diaDownRightIndex;
    }
}

IndexTranslation* lookup_translateIndex( const uint index ) {
    return &indexTranslations[index];
}

//lookup table dims are [64][2], first is location, second is white (0) or black (1)
static void initializePawnLookupTable() {
    uint index;
    for ( uint row = 0; row < 8; ++row ) {
        for ( uint col = 0; col < 8; ++col ) {
            index = row * 8 + col;
            if ( row > 0 ) {
                addMove( row + 1, col - 1, &pawnCaptureLookupTable[index][1] );
                addMove( row + 1, col + 1, &pawnCaptureLookupTable[index][1] );
            }

            if ( row < 7 ) {
                addMove( row - 1, col - 1, &pawnCaptureLookupTable[index][0] );
                addMove( row - 1, col + 1, &pawnCaptureLookupTable[index][0] );
            }
        }
    }
}

static void initializeKingLookupTable() {
    uint64_t moves;
    int row, col;
    for ( uint i = 0; i < 64; ++i ) {
        moves = 0;
        row = i / 8;
        col = i % 8;
        for ( int rOffset = -1; rOffset <= 1; ++rOffset ) {
            for ( int cOffset = -1; cOffset <= 1; ++cOffset ) {
                if ( rOffset == 0 && cOffset == 0 ) {
                    continue;
                }
                addMove( row + rOffset, col + cOffset, &moves );
            }
        }
        kingLookupTable[i] = moves;
    }
}

static void initializeKnightLookupTable() {
    uint64_t moves;
    int row, col, tempRow, tempCol;
    for ( uint i = 0; i < 64; ++i ) {
        moves = 0;
        row = i / 8;
        col = i % 8;
        tempRow = row - 2;
        tempCol = col - 1;
        addMove( tempRow, tempCol, &moves );

        tempCol += 2;
        addMove( tempRow, tempCol, &moves );

        tempRow += 1;
        tempCol += 1;
        addMove( tempRow, tempCol, &moves );

        tempRow += 2;
        addMove( tempRow, tempCol, &moves );

        tempRow += 1;
        tempCol -= 1;
        addMove( tempRow, tempCol, &moves );

        tempCol -= 2;
        addMove( tempRow, tempCol, &moves );

        tempRow -= 1;
        tempCol -= 1;
        addMove( tempRow, tempCol, &moves );

        tempRow -= 2;
        addMove( tempRow, tempCol, &moves );

        knightLookupTable[i] = moves;
    }
}


static void initializeHVLookupTable() {
    char moves;
    for ( uint i = 0; i < 256; ++i ) {
        for ( uint j = 0; j < 8; ++j ) {
            if ( !( i >> ( 7 - j ) & 1 ) ) { //if the (7-j)th bit is not set, no piece there, value is 0
                continue;
            }
            moves = 0;
            for ( int left = j - 1; left >= 0; --left ) {
                moves |= 1 << ( 7 - left );
                if ( i >> ( 7 - left ) & 1 ) { //encounters first other piece
                    break;
                }
            }

            for ( int right = j + 1; right <= 7; ++right ) {
                moves |= 1 << ( 7 - right );
                if ( i >> ( 7 - right ) & 1 ) { //encounters first other piece
                    break;
                }
            }
            hvdLookupTable[i][j] = moves;
        }
    }

}

void initializeLookupTables() {
    srand( time( NULL ) );
    initializeHVLookupTable();
    initializeKnightLookupTable();
    initializeKingLookupTable();
    initializePawnLookupTable();
    initializeIndexTranslations();
}



static void lookup_setHorizontalMoves( uint64_t* const moves, const unsigned char rowBitMap,
                                       const uint rowNumber, const uint colNumber ) {
    //ran into issues when this was a signed char
    unsigned char rowMoveBitMap = hvdLookupTable[( int ) rowBitMap][colNumber];
    if ( rowMoveBitMap == 0 ) { //no moves, which should mean that there is no piece at the specified col number?
        return;
    }
    *moves |= ( ( uint64_t ) rowMoveBitMap ) << ( ( 7 - rowNumber ) * 8 );
}

static void lookup_setVerticalMoves( uint64_t* const moves, const unsigned char colBitMap,
                                     const uint rowNumber, const uint colNumber ) {
    char colMoveBitMap = hvdLookupTable[( int ) colBitMap][rowNumber];
    if ( colMoveBitMap == 0 ) { //no moves, which should mean that there is no piece at the specified col number?
        return;
    }
    for ( uint row = 0; row < 8; ++row ) {
        if ( colMoveBitMap >> ( 7 - row ) & 1 ) {
            addMove( row, colNumber, moves );
        }
    }
}

static void lookup_setDiagonalMoves( uint64_t* const moves, const unsigned char diaBitMapUpRight, 
                                     const unsigned char diaBitMapDownRight,
                                     const uint rowNumber, const uint colNumber ) {
    uint numSquares = rowNumber + colNumber + 1;
    if ( numSquares > 7 ) {
        numSquares = 16 - numSquares;
    }
    uint rowDiff = 7 - rowNumber;
    uint colDiff = colNumber;
    uint offset = rowDiff < colDiff ? rowDiff : colDiff;
    uint firstRow = rowNumber + offset;
    uint firstCol = colNumber - offset;
    char diaMoveBitMap = hvdLookupTable[diaBitMapUpRight][offset]; //diagonals going up and to the right

    for ( uint index = 0; index < numSquares; ++index ) {
        if ( diaMoveBitMap >> ( 7 - index ) & 1 ) {
            addMove( firstRow, firstCol, moves );
        }
        --firstRow;
        firstCol++;
    }

    numSquares = 8 - abs( ( int ) rowNumber - ( int ) colNumber );
    rowDiff = rowNumber;
    colDiff = colNumber;
    offset = rowDiff < colDiff ? rowDiff : colDiff;
    firstRow = rowNumber - offset;
    firstCol = colNumber - offset;
    diaMoveBitMap = hvdLookupTable[diaBitMapDownRight][offset];

    for ( uint index = 0; index < numSquares; ++index ) {
        if ( diaMoveBitMap >> ( 7 - index ) & 1 ) {
            addMove( firstRow, firstCol, moves );
        }
        ++firstRow;
        ++firstCol;
    }
}

uint64_t lookup_getKingMoves( const uint row, const uint col ) {
    if ( row > 7 || col > 7 ) return 0;
    return kingLookupTable[row * 8 + col];
}

uint64_t lookup_getQueenMoves( const Board* const board, const uint row, const uint col ) {
    if ( row > 7 || col > 7 ) return 0;
    uint64_t moves = 0;
    lookup_setVerticalMoves( &moves, board->bitFields.allCols[col], row, col );
    lookup_setHorizontalMoves( &moves, board->bitFields.allRows[row], row, col );
    lookup_setDiagonalMoves( &moves, board->bitFields.allDiasUpRight[rcToUpRight[row][col]], 
                             board->bitFields.allDiasDownRight[rcToDownRight[row][col]],
                             row, col );
    return moves;
}

uint64_t lookup_getRookMoves( const Board* const board, const uint row, const uint col ) {
    if ( row > 7 || col > 7 ) return 0;
    uint64_t moves = 0;
    lookup_setVerticalMoves( &moves, board->bitFields.allCols[col], row, col );
    lookup_setHorizontalMoves( &moves, board->bitFields.allRows[row], row, col );
    return moves;
}

uint64_t lookup_getBishopMoves( const Board* const board, const uint row, const uint col ) {
    if ( row > 7 || col > 7 ) return 0;
    uint64_t moves = 0;
    lookup_setDiagonalMoves( &moves, board->bitFields.allDiasUpRight[rcToUpRight[row][col]], 
                             board->bitFields.allDiasDownRight[rcToDownRight[row][col]],
                             row, col );
    return moves;
}

uint64_t lookup_getKnightMoves( const uint row, const uint col ) {
    if ( row > 7 || col > 7 ) return 0;
    return knightLookupTable[row * 8 + col];
}

//sets moves to only the moves for moving directly up/down, since a pawn
//can't capture with those. Then sets the captures for diagonals
uint64_t lookup_getPawnMoves( const Board* const board, const uint row, const uint col ) {
    bool isWhite = board->pieceMap[row][col].isWhite;
    uint tooFarRow = isWhite ? 0 : 7;
    if ( row == tooFarRow ) {
        return 0;
    }
    int offset = isWhite ? -1 : 1;
    if ( board->pieceMap[row + offset][col].type != NONE ) {
        return 0;
    }
    uint64_t moves = 0;
    addMove( row + offset, col, &moves);
    uint doubleMoveRow = isWhite ? 6 : 1;
    if ( row != doubleMoveRow || board->pieceMap[row + ( offset + offset )][col].type != NONE ) {
        return moves;
    }
    addMove( row + ( offset + offset ), col, &moves );
    return moves;
}

uint64_t lookup_getPawnCaptures( const uint index, const bool isWhite ) {
    return pawnCaptureLookupTable[index][isWhite ? 0 : 1];
}
