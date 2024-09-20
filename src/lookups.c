#include "board.h"
#include "lookups.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

uint64_t kingLookupTable[64] = {0};
uint64_t knightLookupTable[64] = {0};
char hvdLookupTable[256][8] = {0}; //first index: binary representation of row. second index: position of piece being checked
uint rcToUpRight[8][8] = { { 0, 1, 2, 3, 4, 5, 6, 7 },
                           { 1, 2, 3, 4, 5, 6, 7, 8 },
                           { 2, 3, 4, 5, 6, 7, 8, 9 },
                           { 3, 4, 5, 6, 7, 8, 9, 10 },
                           { 4, 5, 6, 7, 8, 9, 10, 11 },
                           { 5, 6, 7, 8, 9, 10, 11, 12 },
                           { 6, 7, 8, 9, 10, 11, 12, 13 },
                           { 7, 8, 9, 10, 11, 12, 13, 14 } };
 uint rcToDownRight[8][8] = { { 7, 8, 9, 10, 11, 12, 13, 14 },
                              { 6, 7, 8, 9, 10, 11, 12, 13 },
                              { 5, 6, 7, 8, 9, 10, 11, 12 },
                              { 4, 5, 6, 7, 8, 9, 10, 11 },
                              { 3, 4, 5, 6, 7, 8, 9, 10 },
                              { 2, 3, 4, 5, 6, 7, 8, 9 },
                              { 1, 2, 3, 4, 5, 6, 7, 8 },
                              { 0, 1, 2, 3, 4, 5, 6, 7 } };

uint64_t pawnLookupTable[64][2] = {0};

void printMoves( uint64_t moves, uint indexToCheck, char symbol ) {
    printf( "%u\n", indexToCheck );
    for ( uint r = 0; r < 8; ++r ) {
        for ( uint c = 0; c < 8; ++c ) {
            uint index = r * 8  + c;
            if ( index == indexToCheck ) {
                printf( "%c", symbol );
            } else {
                printf( "%i", ( int ) ( ( moves >> ( 63 - index ) ) & 1 ) ); 
            }
        }
        printf( "\n" );
    }
    printf( "\n\n" );
}

static void addMove( int row, int col, uint64_t *moves ) {
    if ( row >= 0 && row <= 7 && col >= 0 && col <= 7 ) {
        uint64_t temp = 1;
        *moves |= ( temp << ( 63 - ( row * 8 + col ) ) );
    }
}

//lookup table dims are [64][2], first is location, second is white (0) or black (1)
static void initializePawnLookupTable() {
    uint index;
    for ( uint row = 0; row < 8; ++row ) {
        for ( uint col = 0; col < 8; ++col ) {
            index = row * 8 + col;
            if ( row > 0 ) {
                addMove( row + 1, col - 1, &pawnLookupTable[index][1] );
                addMove( row + 1, col, &pawnLookupTable[index][1] );
                addMove( row + 1, col + 1, &pawnLookupTable[index][1] );
                if ( row == 1 ) {
                    addMove( row + 2, col, &pawnLookupTable[index][1] );
                }
            }

            if ( row < 7 ) {
                addMove( row - 1, col - 1, &pawnLookupTable[index][0] );
                addMove( row - 1, col, &pawnLookupTable[index][0] );
                addMove( row - 1, col + 1, &pawnLookupTable[index][0] );
                if ( row == 6 ) {
                    addMove( row - 2, col, &pawnLookupTable[index][0] );
                }
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
    initializeHVLookupTable();
    initializeKnightLookupTable();
    initializeKingLookupTable();
    initializePawnLookupTable();
}



void lookup_setHorizontalMoves( uint64_t *moves, char rowBitMap, uint rowNumber,
                                uint colNumber ) {
    char rowMoveBitMap = hvdLookupTable[rowBitMap][colNumber];
    if ( rowMoveBitMap == 0 ) { //no moves, which should mean that there is no piece at the specified col number?
        return;
    }
    *moves |= ( ( uint64_t ) rowMoveBitMap ) << ( ( 7 - rowNumber ) * 8 );
}

void lookup_setVerticalMoves( uint64_t *moves, char colBitMap, uint rowNumber,
                              uint colNumber ) {
    char colMoveBitMap = hvdLookupTable[colBitMap][rowNumber];
    if ( colMoveBitMap == 0 ) { //no moves, which should mean that there is no piece at the specified col number?
        return;
    }
    for ( uint row = 0; row < 8; ++row ) {
        if ( colMoveBitMap >> ( 7 - row ) & 1 ) {
            addMove( row, colNumber, moves );
        }
    }
}

void lookup_setDiagonalMoves( uint64_t *moves, unsigned char diaBitMapUpRight, 
                              unsigned char diaBitMapDownRight, uint rowNumber, uint colNumber ) {
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

void getKingMoves( uint row, uint col, uint64_t *moves ) {
    if ( row > 7 || col > 7 ) return;
    *moves = kingLookupTable[row * 8 + col];
}

void getQueenMoves( Board *board, uint row, uint col, uint64_t *moves ) {
    if ( row > 7 || col > 7 ) return;
    lookup_setVerticalMoves( moves, board->bitMapCols[col], row, col );
    lookup_setHorizontalMoves( moves, board->bitMapRows[row], row, col );
    lookup_setDiagonalMoves( moves, board->bitMapDiasUpRight[rcToUpRight[row][col]], 
                             board->bitMapDiasDownRight[rcToDownRight[row][col]],
                             row, col );
}

void getRookMoves( Board *board, uint row, uint col, uint64_t *moves ) {
    if ( row > 7 || col > 7 ) return;
    lookup_setVerticalMoves( moves, board->bitMapCols[col], row, col );
    lookup_setHorizontalMoves( moves, board->bitMapRows[row], row, col );
}

void getBishopMoves( Board *board, uint row, uint col, uint64_t *moves ) {
    if ( row > 7 || col > 7 ) return;
    uint diaUpRightIndex = ( row + col ); //
    uint diaDownRightIndex;
    lookup_setDiagonalMoves( moves, board->bitMapDiasUpRight[rcToUpRight[row][col]], 
                             board->bitMapDiasDownRight[rcToDownRight[row][col]],
                             row, col );
}

void getKnightMoves( uint row, uint col, uint64_t *moves ) {
    if ( row > 7 || col > 7 ) return;
    *moves = knightLookupTable[row * 8 + col];
}

//sets moves to only the moves for moving directly up/down, since a pawn
//can't capture with those. Then sets the captures for diagonals
void getPawnMoves( Board *board, uint row, uint col, uint64_t *moves, uint64_t *captures ) {
    bool isWhite = board->pieceMap[row][col].isWhite;
    if ( isWhite ) {
        if ( row == 0 ) {
            return;
        }
        if ( board->pieceMap[row - 1][col].type == NONE ) {
            addMove( row - 1, col, moves);
            if ( row == 6 && board->pieceMap[row - 2][col].type == NONE ) {
                addMove( row - 2, col, moves );
            }
        }
        //diagonal captures
        if ( col > 0 && board->pieceMap[row - 1][col - 1].type != NONE && !board->pieceMap[row - 1][col - 1].isWhite ) {
            addMove( row - 1, col - 1, captures );
        }
        if ( col < 7 && board->pieceMap[row - 1][col + 1].type != NONE && !board->pieceMap[row - 1][col + 1].isWhite ) {
            addMove( row - 1, col + 1, captures );
        }
    } else {
        if ( row == 7 ) {
            return;
        }
        if ( board->pieceMap[row + 1][col].type == NONE ) {
            addMove( row + 1, col, moves);
            if ( row == 1 && board->pieceMap[row + 2][col].type == NONE ) {
                addMove( row + 2, col, moves );
            }
        }
        //diagonal captures
        if ( col > 0 && board->pieceMap[row + 1][col - 1].type != NONE && !board->pieceMap[row + 1][col - 1].isWhite ) {
            addMove( row + 1, col - 1, captures );
        }
        if ( col < 7 && board->pieceMap[row + 1][col + 1].type != NONE && !board->pieceMap[row + 1][col + 1].isWhite ) {
            addMove( row + 1, col + 1, captures );
        }
    }
}
