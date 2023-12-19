#ifndef CHESS_BOARD_
#define CHESS_BOARD_

#include <cstdint>
#include <inttypes.h>
#include <stdbool.h>

typedef struct Board {
    int64_t bitMap; //what parts of the board are occupied, each square is 1 bit
    int64_t pieceMap[3]; //actual pieces on the board
    bool    whiteToMove; 
} Board;



#endif
