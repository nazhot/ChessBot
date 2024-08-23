#include "lookups.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

static void addMove( int row, int col, uint64_t *moves ) {
    if ( row >= 0 && row <= 7 && col >= 0 && col <= 7 ) {
        uint64_t temp = 1;
        *moves |= ( temp << ( 63 - ( row * 8 + col ) ) );
    }
}

static void initializeKnightLookupTable() {
    uint64_t knightLookup[64] = {0};
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
 /* 
        printf( "%u (%lu)\n", i, moves );
        for ( uint r = 0; r < 8; ++r ) {
            for ( uint c = 0; c < 8; ++c ) {
                uint index = r * 8  + c;
                if ( index == i ) {
                    printf( "K" );
                } else {
                    printf( "%i", ( int ) ( ( moves >> ( 63 - index ) ) & 1 ) ); 
                }
            }
            printf( "\n" );
        }
        printf( "\n\n" );
*/
        knightLookup[i] = moves;
    }
}

static void initializeHVLookupTable() {
    char hvLookup[256][8] = {0}; //first index: binary representation of row. second index: position of piece being checked
    char moves;
    for ( uint i = 0; i < 256; ++i ) {
/*
        for ( int k = 7; k >= 0; --k ) {
            printf( "%i", i >> k & 1 );
        }
        printf( ": " );
*/
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
/*
            for ( int k = 7; k >= 0; --k ) {
                printf( "%i", moves >> k & 1 );
            }
            printf( "(%u) ", j );
*/
            hvLookup[i][j] = moves;
        }
//        printf( "\n" );
    }

}

void initializeLookupTables() {
    initializeHVLookupTable();
    initializeKnightLookupTable();
}
