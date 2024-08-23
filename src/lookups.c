#include "lookups.h"
#include <stdio.h>
#include <stdlib.h>

void initializeLookupTables() {
    char hvLookup[256][8] = {0}; //first index: binary representation of row. second index: position of piece being checked
    char moves;
    for ( uint i = 0; i < 256; ++i ) {
        for ( int k = 7; k >= 0; --k ) {
            printf( "%i", i >> k & 1 );
        }
        printf( ": " );
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
            for ( int k = 7; k >= 0; --k ) {
                printf( "%i", moves >> k & 1 );
            }
            printf( "(%u) ", j );
            hvLookup[i][j] = moves;
        }
        printf( "\n" );
    }

}
