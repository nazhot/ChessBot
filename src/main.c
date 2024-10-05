#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "lookups.h"
#include "server.h"

int main( int argc, char *argv[] ) {

    //int serverFD = server_startLocal();
    //server_listen();
    initializeLookupTables();

    Board *board = board_initialize();
    //board_print( board );

    //board_decideAndMakeMove( board );
    board_playGame( board );

    free( board );
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
