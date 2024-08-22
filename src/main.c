#include <stdio.h>
#include <stdlib.h>
#include "board.h"

int main( int argc, char *argv[] ) {
    Board *board = board_initialize();
    board_print( board );
    uint32_t moves = board_getPieceDirectionMoves( board, 6, 1 );
    board_printDirectionMoves( moves );
    return 0;
}
