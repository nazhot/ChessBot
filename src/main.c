#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "lookups.h"
#include "server.h"
#include <string.h>
#include <assert.h>

static int randomIndex( const uint size ) {
    if ( ( size - 1 ) == RAND_MAX ) {
        return rand();
    }
    int end = RAND_MAX / size; // truncate skew
    assert (end > 0);
    end *= size;

    int r;
    while ((r = rand()) >= end);

    return r % size;
}

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

Move randomMove( Board *board ) {
    uint numMoves;
    Move *moves;
    Move decidedMove;
    moves = board_getMovesForCurrentSide( board, &numMoves, true );
    uint index = randomIndex( numMoves );
    memcpy( &decidedMove, &moves[index], sizeof( Move ) );
    free( moves );
    return decidedMove;
}

Move captureHeavy( Board *board ) {
    static uint pieceWeightMove[7] = { [NONE] = 0, [PAWN] = 1, [KNIGHT] = 3, [BISHOP] = 3,
                                    [ROOK] = 3, [KING] = 1, [QUEEN] = 5 };
    static uint pieceWeightCapture[7] = { [NONE] = 0, [PAWN] = 3, [KNIGHT] = 5, [BISHOP] = 5,
                                    [ROOK] = 5, [KING] = 57, [QUEEN] = 10 };
    uint numMoves;
    Move *moves;
    Move decidedMove;
    moves = board_getMovesForCurrentSide( board, &numMoves, true );
    uint decidedIndex = 0;
    uint highestScore = 0;
    uint currentScore;
    for ( uint i = 0; i < numMoves; ++i ) {
        currentScore = 0;
        switch( moves[i].moveType ) {
            case MOVE_NORMAL:
                currentScore += pieceWeightMove[moves[i].pieceType];
                break;
            case MOVE_CASTLE:
                currentScore += 2;
                break;
            case MOVE_CAPTURE:
                currentScore += pieceWeightCapture[moves[i].captureType];
                break;
            case MOVE_PROMOTION:
                currentScore += pieceWeightCapture[moves[i].promotionType] * 2;
                break;
        }
        if ( currentScore > highestScore ) {
            highestScore = currentScore;
            decidedIndex = i;
        }
    }
    memcpy( &decidedMove, &moves[decidedIndex], sizeof( Move ) );
    free( moves );
    return decidedMove;
}


int main( int argc, char *argv[] ) {

    //int serverFD = server_startLocal();
    //server_listen();
    initializeLookupTables();


    GameStatus status = board_playGame( captureHeavy, randomMove );
    if ( status == END_BLACK_WON ) {
        printf( "Black won!!!\n" );
    } else if ( status == END_WHITE_WON ) {
        printf( "White won!!!\n" );
    } else if ( status == END_STALEMATE ) {
        printf( "Draw!!!\n" );
    } else if ( status == END_MAX_MOVES ) {
        printf( "Draw, MAX moves!!\n" );
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
