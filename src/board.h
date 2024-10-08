#ifndef CHESS_BOARD_
#define CHESS_BOARD_

#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pieces.h"

typedef struct IndexTranslation {
    uint index;
    uint row;
    uint col;
    uint diaUpRight;
    uint diaUpRightIndex;
    uint diaDownRight;
    uint diaDownRightIndex;
} IndexTranslation;

typedef enum MoveType {
    MOVE_NORMAL,
    MOVE_CASTLE,
    MOVE_CHECK,
    MOVE_CHECKMATE,
    MOVE_CAPTURE,
    MOVE_PROMOTION
} MoveType;

typedef enum CaptureType {
    CAPTURE_NORMAL,
    CAPTURE_EN_PASSANT,
    CAPTURE_CHECK,
    CAPTURE_CHECKMATE
} CaptureType;

typedef enum MoveDirection {
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} MoveDirection;

//WIP
//Used to fully describe a move, including information that should help in
//grading moves and deciding which to choose. Need to use it more in contexts
//to see what, if anything, should change/be added.
typedef struct Move {
    bool whiteMove;
    char algebraicNotation[10];
    PieceType pieceType;
    MoveType moveType;
    CaptureType captureType;
    union {
        PieceType pieceCaptured;
        MoveDirection castleDirection;
        PieceType promotionType; 
    };
    uint dstRow;
    uint dstCol;
    uint srcRow;
    uint srcCol;
} Move;

//Has all of the information about a board. The bitFields member contains all of
//the different bit representations of the current board. "Board" variables are
//bit representations of the full board. "Board" >> ( 63 - index ) & 1 = a piece is there.
//All char variables use char >> ( 7 - index ) to get the bit. DiasUpRight starts
//in the top left of the board, travels to the right. DiasDownRight starts in the bottom
//left and moves right.
typedef struct Board {
    struct {
        uint64_t allBoard;
        uint64_t whtBoard;
        uint64_t blkBoard;

        unsigned char allRows[8];
        unsigned char allCols[8];
        unsigned char allDiasUpRight[15];
        unsigned char allDiasDownRight[15];

        unsigned char whtRows[8];
        unsigned char whtCols[8];
        unsigned char whtDiasUpRight[15];
        unsigned char whtDiasDownRight[15];

        unsigned char blkRows[8];
        unsigned char blkCols[8];
        unsigned char blkDiasUpRight[15];
        unsigned char blkDiasDownRight[15];
        
    } bitFields;
    Piece pieceMap[8][8]; //actual pieces on the board
    bool whiteToMove; 
    bool whiteInCheck;
    bool blackInCheck;
    Move pastMoves[256];
    uint numPastMoves;
    IndexTranslation whiteKing;
    IndexTranslation blackKing;
    bool gameOver;
} Board; 

/**
 * Initialize board with the standard starting layout
 *
 * @return newly created Board
 */
Board* board_initialize();
void board_clear( Board *board );
void board_getMovesForSide( Board *board, bool whiteToMove, Move *moveArray, 
                            uint moveArraySize );
Move* board_getMovesForCurrentSide( Board *board, uint *numMoves );
Move* board_getMovesForOppositeSide( Board *board, uint *numMoves );
void board_makeMove( Board *board, Move *move );
void board_print( Board *board );
void board_decideAndMakeMove( Board *board );
void board_printBitField( char bitField, char *text );
void board_playGame( Board *board );

#endif
