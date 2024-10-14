#ifndef CHESS_BOARD_
#define CHESS_BOARD_

#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pieces.h"

#define GAME_MAX_MOVES 1024

typedef enum CheckType {
    NO_CHECK,
    CHECK_AGAINST_ME,
    CHECK_FOR_ME,
    CHECKMATE,
    STALEMATE
} CheckType;

typedef enum GameStatus {
    GAME_RUNNING,
    END_STALEMATE,
    END_WHITE_WON,
    END_BLACK_WON,
    END_MAX_MOVES
} GameStatus;

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
    MOVE_CAPTURE,
    MOVE_PROMOTION
} MoveType;

typedef enum CaptureType {
    CAPTURE_NORMAL,
    CAPTURE_EN_PASSANT,
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
    PieceType pieceType;
    MoveType moveType;
    CaptureType captureType;
    union {
        PieceType pieceCaptured;
        MoveDirection castleDirection;
        PieceType promotionType; 
    };
    uint dstRow: 4;
    uint dstCol: 4;
    uint srcRow: 4;
    uint srcCol: 4;
    CheckType checkType;
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
    Move lastMove;
    IndexTranslation whiteKing;
    IndexTranslation blackKing;
} Board; 

//contains an entire game, meant to remove the large array from the board
//for when I start checking lots of moves in the future
typedef struct ChessGame {
    Board *board;
    uint numPastMoves;
    Move pastMoves[GAME_MAX_MOVES];
    GameStatus status;
} ChessGame;

/**
 * Initialize board with the standard starting layout
 *
 * @return newly created Board
 */

typedef Move (MoveDecider)(Board *board );

Board* board_initialize();
void board_clear( Board* const board );
Move* board_getMovesForCurrentSide( Board* const board, uint* const numMoves, bool checkForCheck );
Move* board_getMovesForOppositeSide( Board* const board, uint* const numMoves );
GameStatus board_makeMove( Board* const board, const Move* const move );
void board_print( const Board* const board );
void board_printBitField( const char bitField, const char *text );
void board_printMove( const Move* const move );
GameStatus board_playGame( MoveDecider whiteMoveDecider, MoveDecider blackMoveDecider );

#endif
