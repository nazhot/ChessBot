#ifndef CHESS_BOARD_
#define CHESS_BOARD_

#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pieces.h"

typedef struct Move {
    char algebraicNotation[10];
    Piece pieceCaptured;
    char destinationSquare[2];
    char sourceSquare[2];
} Move;

typedef struct Board {
    uint64_t bitMap; //what parts of the board are occupied, each square is 1 bit
    uint64_t whiteBitMap;
    uint64_t blackBitMap;
    Piece pieceMap[8][8]; //actual pieces on the board
    bool    whiteToMove; 
} Board; 

/**
 * Initialize board with the standard starting layout
 *
 * @return newly created Board
 */
Board* board_initialize();
void board_getMovesForPiece( Board *board, char sourceSquare[2], Move *moveArray,
                             uint moveArraySize );
void board_getMovesForSide( Board *board, bool whiteToMove, Move *moveArray, 
                            uint moveArraySize );
void board_makeMove( Board *board, Move *move );

uint32_t board_getPieceDirectionMoves( Board *board, uint row, uint col );

#endif
