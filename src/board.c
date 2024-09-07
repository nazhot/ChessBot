#include <assert.h>
#include "board.h"
#include "lookups.h"
#include "pieces.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum Direction {
    NORTH,
    NORTH_EAST,
    EAST,
    SOUTH_EAST,
    SOUTH,
    SOUTH_WEST,
    WEST,
    NORTH_WEST
} Direction;


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
    board->blackBitMap = 0b1111111111111111000000000000000000000000000000000000000000000000;
    board->whiteBitMap = 0b0000000000000000000000000000000000000000000000001111111111111111;
    //need to find way to distinguish black vs. white
    board->pieceMap[0][0] = initializePiece( ROOK, false );
    board->pieceMap[0][1] = initializePiece( KNIGHT, false );
    board->pieceMap[0][2] = initializePiece( BISHOP, false );
    board->pieceMap[0][3] = initializePiece( QUEEN, false );
    board->pieceMap[0][4] = initializePiece( KING, false );
    board->pieceMap[0][5] = initializePiece( BISHOP, false );
    board->pieceMap[0][6] = initializePiece( KNIGHT, false );
    board->pieceMap[0][7] = initializePiece( ROOK, false );
    for ( uint i = 0; i < 8; ++i ) {
        board->pieceMap[1][i] = initializePiece( PAWN, false );
        board->pieceMap[6][i] = initializePiece( PAWN, true );
    }

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

static void addDirectionMove( uint32_t *move, uint numSquares, Direction direction, bool capture ) {
    int binaryToEncode = capture ? 8 + numSquares : numSquares;
    *move |= binaryToEncode << ( direction * 4 );
}

void board_printDirectionMoves( uint32_t moves ) {
    static char *directions[] = { [NORTH] = "N", [NORTH_EAST] = "NE", [EAST] = "E",
                                 [SOUTH_EAST] = "SE", [SOUTH] = "S", [SOUTH_WEST] = "SW",
                                 [WEST] = "W", [NORTH_WEST] = "NW" };
    uint move;
    bool capture;
    uint distance;
    for ( uint i = 0; i < 8; ++i ) {
        move = moves >> ( i * 4 ) & 15;
        if ( move == 0 ) continue;
        capture = move >= 8;
        distance = capture ? move - 8 : move;
        printf( "Move up to %u %s%s", distance, directions[i], capture ? " (capture)\n" : "\n" );
    }
}

void board_getPieceDirectionMoves( Board *board, uint row, uint col,
                                       uint64_t *moveBitMap, uint64_t *captureBitMap ) {
    Piece piece = board->pieceMap[row][col];
    if ( piece.type == NONE ) return;
    uint64_t oppositeBitMap = piece.isWhite ? board->blackBitMap : board->whiteBitMap;
    uint32_t moves = 0;
    int boardNumber = row * 8 + col;
    switch ( piece.type ) {
        case PAWN:
            boardNumber -= 9; //move position to check up and to the left
            if ( col > 0 ) {
                //check if opponent is up and to the left
                if ( oppositeBitMap >> boardNumber & 1 ) {
                    *moveBitMap ^= 1 << ( 81 - boardNumber );
                    *captureBitMap ^= 1 << ( 81 - boardNumber );
                    //addDirectionMove( &moves, 1, NORTH_WEST, true );
                }
            }
            ++boardNumber; //move position to the right
            //check if anyone is directly up
            bool noPieceOneUp = !( board->bitMap >> boardNumber & 1 );
            bool noPieceTwoUp = ( noPieceOneUp ) && ( piece.numMoves == 0 ) && !( board->bitMap >> ( boardNumber - 8 ) & 1 );
            *moveBitMap ^= noPieceOneUp << ( 81 - boardNumber );
            *moveBitMap ^= noPieceTwoUp << ( 81 - boardNumber );
            if ( col < 7 ) {
                ++boardNumber;
                //check if opponent is up and to the right
                if ( oppositeBitMap >> boardNumber & 1 ) {
                    *moveBitMap ^= 1 << ( 81 - boardNumber );
                    *captureBitMap ^= 1 << ( 81 - boardNumber );
                    //addDirectionMove( &moves, 1, NORTH_EAST, true ); 
                }
            }
            //need to check en passante
            break;
        case ROOK:
            bool firstFound = false;
            bool secondFound = false;
            int firstBoardNumber = boardNumber - 8; //going up
            int secondBoardNumber = boardNumber + 8; //going down
            uint firstCount = 0;
            uint secondCount = 0;
            while ( !firstFound || !secondFound ) {
                if ( !firstFound && ( board->bitMap >> boardNumber & 1 ) ) {
                    bool opponent = oppositeBitMap >> boardNumber & 1;
                    addDirectionMove( &moves, opponent ? firstCount + 1 : firstCount, NORTH, opponent ); 
                }

                ++firstCount;
                ++secondCount;
            }
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
}

void board_print( Board *board ) {
    static char pieceSymbols[] = { [NONE] = ' ', [PAWN] = 'P', [KNIGHT] = 'N', 
                                   [BISHOP] = 'B', [ROOK] = 'R', [QUEEN] = 'Q',
                                   [KING] = 'K' };
    printf( "---------------------------------\n" );
    for ( uint row = 0; row < 8; ++row ) {
        printf( "|" );
        for ( uint col = 0; col < 8; ++col ) {
            Piece piece = board->pieceMap[row][col];
            char symbol = pieceSymbols[piece.type];
            if ( piece.isWhite ) symbol += 32; 
            printf( " %c |", symbol );
        }
        printf( "\n---------------------------------\n" );
    }
}

static void board_getCapturesFromMoves( Board *board, uint64_t *captures, 
                                        uint64_t *moves, bool whiteToMove ) {
    uint64_t opponentPieceBitMap = whiteToMove ? board->blackBitMap : board->whiteBitMap;
    uint64_t friendlyPieceBitMap = whiteToMove ? board->whiteBitMap : board->blackBitMap;
    *captures = *moves & opponentPieceBitMap; //bits are set on the squares where captures occur
    *moves = *moves ^ friendlyPieceBitMap & *moves; 
}

void board_printMovesCount( Board *board ) {
    uint64_t moves;
    uint numMoves;
    char symbol;
    for ( uint row = 0; row < 8; ++row ) {
        for ( uint col = 0; col < 8; ++col ) {
            moves = 0;
            switch( board->pieceMap[row][col].type ) {
                case NONE:
                case PAWN:
                    continue;
                case KNIGHT:
                    getKnightMoves( row, col, &moves );
                    symbol = 'N';
                    break;
                case QUEEN:
                    getQueenMoves( board, row, col, &moves );
                    symbol = 'Q';
                    break;
                case BISHOP:
                    getBishopMoves( board, row, col, &moves );
                    symbol = 'B';
                    break;
                case KING:
                    getKingMoves( row, col, &moves );
                    symbol = 'K';
                    break;
                case ROOK:
                    getRookMoves( board, row, col, &moves );
                    symbol = 'R';
                    break;
            }
            numMoves = 0;
            for ( uint i = 0; i < 64; ++i ) {
                numMoves += moves >> i & 1 ? 1 : 0;
            }
            printf( "%c%c (%u, %u): %u\n", board->pieceMap[row][col].isWhite ? 'w' : 'b',
                                           symbol, row, col, numMoves );
            //printMoves( moves, row * 8 + col, symbol );
        }
    }
}

void board_decideAndMakeMove( Board *board ) {
    //get all of the moves of the currently moving side
    //do whatever algorithm to determine which move to do
    //make the move
    //update all the pieces within board
    //update all of the various bit fields within board
}
