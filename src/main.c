#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "lookups.h"

int main( int argc, char *argv[] ) {
    Board *board = board_initialize();
    board_print( board );
    //uint32_t moves = board_getPieceDirectionMoves( board, 6, 1 );
    //board_printDirectionMoves( moves );

    initializeLookupTables();
    uint64_t moves = 0;
    char diaBitMapUpRight = 0b01010000;
    char diaBitMapDownRight = 0b11001000;
    lookup_setDiagonalMoves( &moves, diaBitMapUpRight, diaBitMapDownRight, 4, 4 );
    board_printMovesCount( board );

    return 0;
}
