#include <assert.h>
#include "board.h"
#include "pieces.h"
#include <stdio.h>
#include <stdlib.h>

static Piece initializePiece( PieceType type, bool isWhite ) {
    return ( Piece ) { .type = type,
                       .numMoves = 0,
                       .isWhite = isWhite };
}

Board* board_initialize() {
    Board *board = malloc( sizeof( Board ) );
    if ( !board ) {
        fprintf( stderr, "Could not initialize board!" );
        exit( 1 );
    }
    
    board->whiteToMove = true;
    board->bitMap = 0b1111111111111111000000000000000000000000000000001111111111111111;
    //need to find way to distinguish black vs. white
    board->pieceMap[0][0] = initializePiece( ROOK, false );
    board->pieceMap[0][1] = initializePiece( KNIGHT, false );
    board->pieceMap[0][2] = initializePiece( BISHOP, false );
    board->pieceMap[0][3] = initializePiece( QUEEN, false );
    board->pieceMap[0][4] = initializePiece( KING, false );
    board->pieceMap[0][5] = initializePiece( BISHOP, false );
    board->pieceMap[0][6] = initializePiece( KNIGHT, false );
    board->pieceMap[0][7] = initializePiece( ROOK, false );

    board->pieceMap[7][0] = initializePiece( ROOK, true );
    board->pieceMap[7][1] = initializePiece( KNIGHT, true );
    board->pieceMap[7][2] = initializePiece( BISHOP, true );
    board->pieceMap[7][3] = initializePiece( QUEEN, true );
    board->pieceMap[7][4] = initializePiece( KING, true );
    board->pieceMap[7][5] = initializePiece( BISHOP, true );
    board->pieceMap[7][6] = initializePiece( KNIGHT, true );
    board->pieceMap[7][7] = initializePiece( ROOK, true );

    return board;
}

static Piece* board_getPieceFromSourceSquare( Board *board, const char row, const char col ) {
   return &board->pieceMap[row][col];
}

static uint64_t board_getBlackBitMap( Board *board ) {
    return board->bitMap ^ board->whiteBitMap;
}

void board_getMovesForPiece( Board *board, char sourceSquare[2], Move *moveArray,
                             uint moveArraySize ) {
    Piece *piece = board_getPieceFromSourceSquare( board, sourceSquare[0], sourceSquare[1] );
    if ( piece->type == NONE ) return;
    int direction = board->whiteToMove ? -1 : 1;
    switch( piece->type ) {
        case PAWN:
            if ( ( board->whiteToMove && sourceSquare[0] > 0 ) ||
                 ( !board->whiteToMove && sourceSquare[0] < 7 ) ) {
                if ( board_getPieceFromSourceSquare( board, sourceSquare[0] + direction, sourceSquare[1] )->type == NONE ) {
                   //pawn can move one forward 
                }
            }
            if ( piece->numMoves == 0 ) {

            }
            break;
        case ROOK:
            break;
        case KNIGHT:
            break;
        case QUEEN:
            break;
        case BISHOP:
            break;
        case KING:
            break;
        default:
            return;
    }
}

void board_getMovesForSide( Board *board, bool whiteToMove, Move *moveArray, 
                            uint moveArraySize ) {

}

void board_makeMove( Board *board, Move *move ) {

}

uint32_t board_getPieceDirectionMoves( Board *board, uint row, uint col ) {
    Piece piece = board->pieceMap[row][col];
    if ( piece.type == NONE ) return 0;
    uint64_t oppositeBitMap = piece.isWhite ? board->blackBitMap : board->whiteBitMap;
    uint32_t moves = 0;
    uint boardNumber = row * 8 + col;
    switch ( piece.type ) {
        case PAWN:
            boardNumber -= 9; //move position to check up and to the left
            if ( col > 0 ) {
                //check if opponent is up and to the left
                if ( oppositeBitMap >> boardNumber & 1 ) {
                    moves |= 9 << 28; //magic number 9 is 1001 for move1, capture
                }
            }
            ++boardNumber; //move position to the right
            //check if anyone is directly up
            bool noPieceOneUp = !( board->bitMap >> boardNumber & 1 );
            bool noPieceTwoUp = ( noPieceOneUp ) && ( piece.numMoves == 0 ) && !( board->bitMap >> ( boardNumber - 8 ) & 1 );
            if ( noPieceTwoUp ) {
                moves |= 2;
            } else {
                moves |= noPieceOneUp;
            }
            if ( col < 7 ) {
                ++boardNumber;
                //check if opponent is up and to the left
                if ( oppositeBitMap >> boardNumber & 1 ) {
                    moves |= 9 << 4;
                }
            }
            //need to check en passante
            break;
        case ROOK:
            break;
        case KNIGHT:
            break;
        case QUEEN:
            break;
        case BISHOP:
            break;
        case KING:
            break;
        default:
            break;
    }

    return moves;
}
