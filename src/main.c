#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "lookups.h"
#include "server.h"
#include <string.h>

Move testDecider( Board *board ) {
    uint numMoves;
    Move *moves;
    Move decidedMove;
    moves = board_getMovesForCurrentSide( board, &numMoves, true );
    for ( uint i = 0; i < numMoves; ++i ) {
        printf( "%u: ", i );
        board_printMove( &moves[i] );
    }
    uint index = numMoves;
    char buffer[5];
    while ( index >= numMoves ) {
        printf( "\nChoose an index from 0-%u: ", numMoves - 1 );
        fgets( buffer, 5, stdin );
        index = atoi( buffer );
    }
    board_printMove( &moves[index] );
    memcpy( &decidedMove, &moves[index], sizeof( Move ) );
    free( moves );
    return decidedMove;
}


int main( int argc, char *argv[] ) {

    //int serverFD = server_startLocal();
    //server_listen();
    initializeLookupTables();


    GameStatus status = board_playGame( testDecider, testDecider );
    if ( status == END_BLACK_WON ) {
        printf( "Black won!!!\n" );
    } else if ( status == END_WHITE_WON ) {
        printf( "White won!!!\n" );
    } else if ( status == END_STALEMATE ) {
        printf( "Draw!!!\n" );
    }
/*
    Board *testBoard = board_initialize();
    for ( uint i = 0; i < 8; ++i ) {
        for ( uint j = 0; j < 8; ++j ) {
            board_clear( testBoard );
            testBoard->pieceMap[i][j].type = QUEEN;
            testBoard->pieceMap[i][j].isWhite = true;
            board_decideAndMakeMove( testBoard );
        }
    }
*/  

    return 0;
}
