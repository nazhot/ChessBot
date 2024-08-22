#include <stdio.h>
#include <stdlib.h>
#include "board.h"

int main( int argc, char *argv[] ) {
    Board *board = board_initialize();
    board_print( board );
    return 0;
}
