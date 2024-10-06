#ifndef CHESS_PIECES_
#define CHESS_PIECES_

#include <stdbool.h>
#include <stdlib.h>

typedef enum PieceType {
    NONE = 0,
    PAWN = 1,
    KNIGHT = 2,
    BISHOP = 3,
    ROOK = 4,
    QUEEN = 5 ,
    KING = 6
} PieceType;

typedef struct Piece {
    PieceType type;
    uint numMoves;
    bool isWhite;
} Piece;

#endif
