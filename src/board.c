#include <assert.h>
#include "board.h"
#include "pieces.h"
#include <stdio.h>
#include <stdlib.h>

struct Board* board_initialize() {
    struct Board *board = malloc( sizeof( struct Board ) );
    if ( !board ) {
        fprintf( stderr, "Could not initialize board!" );
        exit( 1 );
    }
    
    board->whiteToMove = true;
    board->bitMap = 0b1111111111111111000000000000000000000000000000001111111111111111;
    //need to find way to distinguish black vs. white
    board->pieceMap[0][0] = ROOK;
    board->pieceMap[0][1] = KNIGHT;
    board->pieceMap[0][2] = BISHOP;
    board->pieceMap[0][3] = QUEEN;
    board->pieceMap[0][4] = KING;
    board->pieceMap[0][5] = BISHOP;
    board->pieceMap[0][6] = KNIGHT;
    board->pieceMap[0][7] = ROOK;

    board->pieceMap[7][0] = ROOK;
    board->pieceMap[7][1] = KNIGHT;
    board->pieceMap[7][2] = BISHOP;
    board->pieceMap[7][3] = KING;
    board->pieceMap[7][4] = QUEEN;
    board->pieceMap[7][5] = BISHOP;
    board->pieceMap[7][6] = KNIGHT;
    board->pieceMap[7][7] = ROOK;

    return board;
}
