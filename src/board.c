#include <stdio.h>
#include <stdlib.h>
#include "board.h"



struct Board* board_initialize() {
    struct Board *board = malloc( sizeof( struct Board ) );
    if ( !board ) {
        fprintf( stderr, "Could not initialize board!" );
        exit( 1 );
    }
    
    board->whiteToMove = true;
    board->bitMap = 0b1111111111111111000000000000000000000000000000001111111111111111;

    return board;
}
