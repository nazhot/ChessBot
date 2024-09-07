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


//consider moving each bitmap to be (3) char[8]'s
//there would be one for rows, one for columns, one for diagonals
//this would allow move calculation to be a lookup value, rather than a looping
//calculation every time
//there would be a size 256 array for each possible combination of pieces in row/col/diagonal
//that would point to another array for each possible position the piece is in,
//that would then point to the valid moves for that piece
//
//to figure out capturing, I think you would do the initial lookup with the bit maps for
//all pieces, and then just have to do two checks, one for each side of the row/col/diagonal
//to see if the piece it ran into was an opponent? Idk, that may be the most complicated part
//
//then lookup tables would have a lot of overlap: rows/cols can be used for rook/queen,
//diagonals for bishop/queen, and there can be specific ones for knights, pawns, and the king
//which should also help a lot with the calculation. For the knight I imagine you
//would just make a size 81 array, for each possible position on the board, and it just gives back
//the places it could jump to. XORing with the piece's bit map would remove the places it cannot jump,
//and ANDing it with the opponent's bit map would show the capture points
typedef struct Board {
    char bitMapRows[8];
    char bitMapCols[8];
    char bitMapDiasUpRight[15];
    char bitMapDiasDownRight[15];
    char whiteBitMapRows[8];
    char whiteBitMapCols[8];
    char whiteBitMapDiasUpRight[15];
    char whiteBitMapDiasDownRight[15];
    char blackBitMapRows[8];
    char blackBitMapCols[8];
    char blackBitMapDiasUpRight[15];
    char blackBitMapDiasDownRight[15];
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
void board_printDirectionMoves( uint32_t moves );
//return value has bit set to 1 if 
void board_getPieceDirectionMoves( Board *board, uint row, uint col,
                                   uint64_t *moveBitMap, uint64_t *captureBitMap );
void board_print( Board *board );
void board_decideAndMakeMove( Board *board );
void board_printMovesCount( Board *board );

#endif
