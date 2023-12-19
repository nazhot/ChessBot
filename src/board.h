#ifndef CHESS_BOARD_
#define CHESS_BOARD_

#include <inttypes.h>
#include <stdbool.h>

struct Board {
    int64_t bitMap; //what parts of the board are occupied, each square is 1 bit
    unsigned char pieceMap[8][8]; //actual pieces on the board
    bool    whiteToMove; 
}; 

/**
 * Initialize board with the standard starting layout
 *
 * @return newly created Board
 */
struct Board* board_initialize();


#endif
