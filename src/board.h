#ifndef CHESS_BOARD_
#define CHESS_BOARD_

#include <inttypes.h>
#include <stdbool.h>

struct Board {
    int64_t bitMap; //what parts of the board are occupied, each square is 1 bit
    int64_t pieceMap[3]; //actual pieces on the board
    bool    whiteToMove; 
}; 

struct Board* board_initialize();


#endif
