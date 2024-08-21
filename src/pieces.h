#ifndef CHESS_PIECES_
#define CHESS_PIECES_

#include <stdbool.h>
#include <stdlib.h>

typedef enum PieceType {
    NONE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
} PieceType;

typedef struct Piece {
    PieceType type;
    uint numMoves;
    bool isWhite;
} Piece;

#endif
