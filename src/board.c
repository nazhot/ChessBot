#include <assert.h>
#include "board.h"
#include "lookups.h"
#include "pieces.h"
#include "tests.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void board_updateBitFieldsFromPieces( Board *board );

void board_printBitField( char bitField, char *text ) {
    printf( "%s: ", text );
    for ( uint i = 0; i < 8; ++i ) {
        printf( "%i", bitField >> ( 7 - i ) & 1 ? 1 : 0 );
    }
}

static Piece initializePiece( PieceType type, bool isWhite ) {
    return ( Piece ) { .type = type,
                       .numMoves = 0,
                       .isWhite = isWhite };
}

static void board_resetBitFields( Board *board ) {
    memset( &board->bitFields, 0, sizeof( board->bitFields ) );
}

static void board_printUint64_t( uint64_t num ) {
    for ( int i = 63; i >= 0; --i ) {
        printf( "%i", num >> i & 1 ? 1 : 0 );
    }
    printf( "\n" );
}

Board* board_initialize() {
    Board *board = malloc( sizeof( Board ) );
    if ( !board ) {
        fprintf( stderr, "Could not initialize board!" );
        exit( 1 );
    }
    
    board->whiteToMove = true;
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
    board->numPastMoves = 0;
    board->whiteInCheck = false;
    board->blackInCheck = false;
    board->gameOver = false;

    board->blackKing = *lookup_translateIndex( 4 );
    board->whiteKing = *lookup_translateIndex( 60 );

    for ( uint row = 2; row < 6; ++row ) {
        for ( uint col = 0; col < 8; ++col ) {
            board->pieceMap[row][col] = initializePiece( NONE, true );
        }
    }

    board_updateBitFieldsFromPieces( board );

    return board;
}

void board_clear( Board *board ) {
    for ( uint i = 0; i < 8; ++i ) {
        for ( uint j = 0; j < 8; ++j ) {
            board->pieceMap[i][j].type = NONE;
        }
    }
    board_updateBitFieldsFromPieces( board );
}

void board_getMovesForSide( Board *board, bool whiteToMove, Move *moveArray, 
                            uint moveArraySize ) {

}

static void board_set8BitFieldIndex( unsigned char *bitField, uint index ) {
    *bitField ^= ( 1 << ( 7 - index ) );
}

static void board_set64BitFieldIndex( uint64_t *bitField, uint index ) {
    *bitField ^= ( ( ( uint64_t ) 1 ) << ( 63 - index ) );
}

static void board_updateBitFieldsFromPieces( Board *board ) {
    board_resetBitFields( board );
    for ( uint i = 0; i < 64; ++i ) {
        IndexTranslation *indexes = lookup_translateIndex( i );
        Piece piece = board->pieceMap[indexes->row][indexes->col];
        if ( piece.type == KING ) {
            if ( piece.isWhite ) {
                board->whiteKing = *indexes;
            } else {
                board->blackKing = *indexes;
            }
        } else if ( piece.type == NONE ) {
            continue;
        }

        board_set64BitFieldIndex( &board->bitFields.allBoard, i );
        if ( piece.isWhite ) {
            board_set64BitFieldIndex( &board->bitFields.whtBoard, i );
        } else {
            board_set64BitFieldIndex( &board->bitFields.blkBoard, i );
        }
        
        board_set8BitFieldIndex( &board->bitFields.allRows[indexes->row], indexes->col );
        if ( piece.isWhite ) {
            board_set8BitFieldIndex( &board->bitFields.whtRows[indexes->row], indexes->col );
        } else {
            board_set8BitFieldIndex( &board->bitFields.blkRows[indexes->row], indexes->col );
        }

        board_set8BitFieldIndex( &board->bitFields.allCols[indexes->col], indexes->row );
        if ( piece.isWhite ) {
            board_set8BitFieldIndex( &board->bitFields.whtCols[indexes->col], indexes->row );
        } else {
            board_set8BitFieldIndex( &board->bitFields.blkCols[indexes->col], indexes->row );
        }

        board_set8BitFieldIndex( &board->bitFields.allDiasUpRight[indexes->diaUpRight], indexes->diaUpRightIndex );
        if ( piece.isWhite ) {
            board_set8BitFieldIndex( &board->bitFields.whtDiasUpRight[indexes->diaUpRight], indexes->diaUpRightIndex );
        } else {
            board_set8BitFieldIndex( &board->bitFields.blkDiasUpRight[indexes->diaUpRight], indexes->diaUpRightIndex );
        }

        board_set8BitFieldIndex( &board->bitFields.allDiasDownRight[indexes->diaDownRight], indexes->diaDownRightIndex );
        if ( piece.isWhite ) {
            board_set8BitFieldIndex( &board->bitFields.whtDiasDownRight[indexes->diaDownRight], indexes->diaDownRightIndex );
        } else {
            board_set8BitFieldIndex( &board->bitFields.blkDiasDownRight[indexes->diaDownRight], indexes->diaDownRightIndex );
        }
    }
}

void board_makeMove( Board *board, Move *move ) {
    ++board->pieceMap[move->srcRow][move->srcCol].numMoves;
    memcpy( &board->pieceMap[move->dstRow][move->dstCol],
            &board->pieceMap[move->srcRow][move->srcCol], sizeof( Piece ) );
    board->pieceMap[move->srcRow][move->srcCol].type = NONE;
    //update king's position in board
    if ( move->pieceType == KING ) {
        if ( move->whiteMove ) {
            board->whiteKing = *lookup_translateIndex( move->dstRow * 8 + move->dstCol );
        } else {
            board->blackKing = *lookup_translateIndex( move->dstRow * 8 + move->dstCol );
        }
    }
    switch ( move->moveType ) {
        case MOVE_CASTLE:
            if ( move->castleDirection == DIRECTION_LEFT ) {
                //king is taken care of with upper code
                //rook
                memcpy( &board->pieceMap[move->srcRow][3], &board->pieceMap[move->srcRow][0],
                       sizeof( Piece ) );
                board->pieceMap[move->srcRow][0].type = NONE; //remove rook
                ++board->pieceMap[move->srcRow][3].numMoves; //increment rook moves
            } else {
                //king is taken care of with upper code
                //rook
                memcpy( &board->pieceMap[move->srcRow][5], &board->pieceMap[move->srcRow][7],
                       sizeof( Piece ) );
                board->pieceMap[move->srcRow][7].type = NONE; //remove rook
                ++board->pieceMap[move->srcRow][5].numMoves; //increment rook moves
            }
            break;
        case MOVE_CAPTURE:
            if ( move->captureType == CAPTURE_EN_PASSANT ) {
                board->pieceMap[move->srcRow][move->dstCol].type = NONE; //captured piece
            }
            break;
        case MOVE_PROMOTION:
            board->pieceMap[move->dstRow][move->dstCol].type = move->promotionType;
            break;
        default:
            break;
    }
    if ( move->leadsToCheck ) {
        if ( move->whiteMove ) {
            board->blackInCheck = true;
        } else {
            board->whiteInCheck = true;
        }
    } else if ( move->leadsToCheckMate ) {
        board->gameOver = true;
    }
    board_updateBitFieldsFromPieces( board );
    memcpy( &board->pastMoves[board->numPastMoves++], move, sizeof( Move ) );
    board->whiteToMove = !board->whiteToMove;
}

void board_print( Board *board ) {
    static char pieceSymbols[] = { [NONE] = ' ', [PAWN] = 'P', [KNIGHT] = 'N', 
                                   [BISHOP] = 'B', [ROOK] = 'R', [QUEEN] = 'Q',
                                   [KING] = 'K' };
    printf( "   a   b   c   d   e   f   g   h\n" );
    printf( " ---------------------------------\n" );
    for ( uint row = 0; row < 8; ++row ) {
        printf( "%u|" , 8 - row );
        for ( uint col = 0; col < 8; ++col ) {
            Piece piece = board->pieceMap[row][col];
            char symbol = pieceSymbols[piece.type];
            if ( piece.type != NONE && piece.isWhite ) symbol += 32; 
            printf( " %c |", symbol );
        }
        printf( "\n ---------------------------------\n" );
    }
}

static void board_getCapturesFromMoves( Board *board, uint64_t *captures, 
                                        uint64_t *moves, bool isWhite ) {
    uint64_t opponentPieceBitMap = isWhite ? board->bitFields.blkBoard : board->bitFields.whtBoard;
    uint64_t friendlyPieceBitMap = isWhite ? board->bitFields.whtBoard : board->bitFields.blkBoard;
    *captures = *moves & opponentPieceBitMap; //bits are set on the squares where captures occur
    *moves = ( *moves ^ friendlyPieceBitMap ) & *moves; 
}

void board_printAlgebraicMoves( uint64_t moves ) {
    printf( "(" );
    bool firstMove = true;
    for ( uint row = 0; row < 8; ++row ) {
        for ( uint col = 0; col < 8; ++col ) {
            uint index = row * 8 + col;
            if ( moves >> ( 63- index ) & 1 ) {
                if ( !firstMove ) {
                    printf( "," );
                }
                printf( "%c%u", col + 97, 8 - row );
                firstMove = false;
            }
        }
    }
    printf( ")" );
}

static void board_printAlgrebraicFromRowCol( uint row, uint col ) {
    printf( "%c%u", col + 97, 8 - row );
}

static void board_printMove( Move *move ) {
    static char pieceSymbols[] = { [NONE] = ' ', [PAWN] = 'P', [KNIGHT] = 'N', 
                                   [BISHOP] = 'B', [ROOK] = 'R', [QUEEN] = 'Q',
                                   [KING] = 'K' };
    if ( move->moveType == MOVE_CASTLE ) {
        if ( move->castleDirection == DIRECTION_LEFT ) {
            printf( "cLeft\n" );
        } else {
            printf( "cRight\n" );
        }
        return;
    }
    board_printAlgrebraicFromRowCol( move->srcRow, move->srcCol ); 
    printf( "-" );
    board_printAlgrebraicFromRowCol( move->dstRow, move->dstCol ); 
    if ( move->moveType == MOVE_CAPTURE ) {
        printf( " (Capture)" );
    } else if ( move->moveType == MOVE_PROMOTION ) {
        printf( " (%c)", pieceSymbols[move->promotionType] );
    }
    if ( move->leadsToCheck ) {
        printf( " (Check)" );
    }
    printf( "\n" );
}

static void board_undoMove( Board *board ) {
    if ( board->numPastMoves == 0 ){
        return;
    }
    Move lastMove = board->pastMoves[--board->numPastMoves];
    --board->pieceMap[lastMove.dstRow][lastMove.dstCol].numMoves;
    memcpy( &board->pieceMap[lastMove.srcRow][lastMove.srcCol],
            &board->pieceMap[lastMove.dstRow][lastMove.dstCol], sizeof( Piece ) );
    board->pieceMap[lastMove.dstRow][lastMove.dstCol].type = NONE;
    //update king's position in board
    if ( lastMove.pieceType == KING ) {
        if ( lastMove.whiteMove ) {
            board->whiteKing = *lookup_translateIndex( lastMove.srcRow * 8 + lastMove.srcCol );
        } else {
            board->blackKing = *lookup_translateIndex( lastMove.srcRow * 8 + lastMove.srcCol );
        }
    }
    switch ( lastMove.moveType ) {
        case MOVE_CASTLE:
            if ( lastMove.castleDirection == DIRECTION_LEFT ) {
                //king is taken care of with upper code
                //rook
                memcpy( &board->pieceMap[lastMove.srcRow][0], &board->pieceMap[lastMove.srcRow][3],
                       sizeof( Piece ) );
                board->pieceMap[lastMove.srcRow][0].type = ROOK; //remove rook
                --board->pieceMap[lastMove.srcRow][0].numMoves; //increment rook moves
            } else {
                //king is taken care of with upper code
                //rook
                memcpy( &board->pieceMap[lastMove.srcRow][7], &board->pieceMap[lastMove.srcRow][5],
                       sizeof( Piece ) );
                board->pieceMap[lastMove.srcRow][7].type = ROOK; //remove rook
                --board->pieceMap[lastMove.srcRow][7].numMoves; //increment rook moves
            }
            break;
        case MOVE_CAPTURE:
            if ( lastMove.captureType == CAPTURE_EN_PASSANT ) {
                board->pieceMap[lastMove.srcRow][lastMove.dstCol].type = lastMove.pieceCaptured; //captured piece
            } else {
                board->pieceMap[lastMove.dstRow][lastMove.dstCol].type = lastMove.pieceCaptured;
            }
            break;
        case MOVE_PROMOTION:
            board->pieceMap[lastMove.srcRow][lastMove.srcCol].type = PAWN;
            break;
        default:
            break;
    }

    if ( lastMove.leadsToCheck ) {
        if ( lastMove.whiteMove ) {
           board->blackInCheck = false; 
        } else {
            board->whiteInCheck = false;
        }
    } else if ( lastMove.leadsToCheckMate ) {
        board->gameOver = false;
    }
    board->whiteToMove = !board->whiteToMove;

    board_updateBitFieldsFromPieces( board );
}

//currently does not check for if the pawn is at the end of the board, the pawn should
//be promoted, hence there should never be a pawn checking for moves at the end of the board
static void board_addPawnCaptures( Board *board, uint64_t *captures, uint64_t *moves, uint row, uint col ) {
    //diagonal captures
    uint index = row * 8 + col;
    bool isWhite = board->pieceMap[row][col].isWhite;
    uint64_t opponentPieceBitMap = isWhite ? board->bitFields.blkBoard : board->bitFields.whtBoard;
    const uint enPassantRow = isWhite ? 3 : 4;
    if ( col > 0 ) {
        int diaLeftOffset = isWhite ? -9 : 7;
        if ( opponentPieceBitMap >> ( 63 - ( index + diaLeftOffset ) ) & 1 ) {
            *captures ^= ( ( uint64_t ) 1 << ( 63 - ( index + diaLeftOffset ) ) );
            *moves ^= ( ( uint64_t ) 1 << ( 63 - ( index + diaLeftOffset ) ) );
        } else if ( row == enPassantRow && ( opponentPieceBitMap >> ( 63 - ( index - 1 ) ) & 1 ) ) {
            Move lastMove = board->pastMoves[board->numPastMoves - 1];
            if ( lastMove.pieceType == PAWN && lastMove.srcCol == col - 1 && 
                 ( abs( ( int ) lastMove.dstRow - ( int ) lastMove.srcRow ) == 2 ) ) {
                *captures ^= ( ( uint64_t ) 1 << ( 63 - ( index + diaLeftOffset ) ) );
            }
        }
    }

    if ( col < 7 ) {
        int diaRightOffset = isWhite ? -7 : 9;
        if ( opponentPieceBitMap >> ( 63 - ( index + diaRightOffset ) ) & 1 ) {
            *captures ^= ( ( uint64_t ) 1 << ( 63 - ( index + diaRightOffset ) ) );
            *moves ^= ( ( uint64_t ) 1 << ( 63 - ( index + diaRightOffset ) ) );
        } else if ( row == enPassantRow && ( opponentPieceBitMap >> ( 63 - ( index + 1 ) ) & 1 ) ) {
            Move lastMove = board->pastMoves[board->numPastMoves - 1];
            if ( lastMove.pieceType == PAWN && lastMove.srcCol == col + 1 && 
                 ( abs( ( int ) lastMove.dstRow - ( int ) lastMove.srcRow ) == 2 ) ) {
                *captures ^= ( ( uint64_t ) 1 << ( 63 - ( index + diaRightOffset ) ) );
            }
        }
    }
}

static bool board_checkCastle( Board *board, MoveDirection direction ) {
    bool isWhite = board->whiteToMove;
    uint checkRow = isWhite ? 7 : 0;
    uint rookCheckCol = direction == DIRECTION_LEFT ? 0 : 7;
    uint blockCheckCol1 = direction == DIRECTION_LEFT ? 1 : 6;
    uint blockCheckCol2 = direction == DIRECTION_LEFT ? 2 : 5;
    uint blockCheckCol3 = 3;
    //still need to check if king is in check, and make sure it does not pass through/
    //finish on a square attacked by a piece
    return ( board->pieceMap[checkRow][4].type == KING &&
             board->pieceMap[checkRow][4].numMoves == 0 &&
             board->pieceMap[checkRow][rookCheckCol].type == ROOK &&
             board->pieceMap[checkRow][rookCheckCol].numMoves == 0 &&
             board->pieceMap[checkRow][blockCheckCol1].type == NONE &&
             board->pieceMap[checkRow][blockCheckCol2].type == NONE &&
             ( direction == DIRECTION_RIGHT || 
               board->pieceMap[checkRow][blockCheckCol3].type == NONE ) );
}

uint64_t board_getMoveBitFieldForPiece( Board *board, uint row, uint col ) {
    if ( board->pieceMap[row][col].type == NONE ) {
        return 0;
    }

    switch( board->pieceMap[row][col].type ) {
        case NONE:
        case PAWN:
            return getPawnMoves( board, row, col );
        case KNIGHT:
            return getKnightMoves( row, col );
        case QUEEN:
            return getQueenMoves( board, row, col );
        case BISHOP:
            return getBishopMoves( board, row, col );
        case KING:
            return getKingMoves( row, col );
        case ROOK:
            return getRookMoves( board, row, col );
    }
    return 0;
}

bool board_moveLeadsToCheck( Board *board, Move *move ) {
    Piece lastPieceMap[8][8] = {0};
    memcpy( lastPieceMap, board->pieceMap, sizeof( Piece ) * 64 );
    board_makeMove( board, move );
    bool leadsToCheck = false;
    uint64_t moves;
    IndexTranslation kingPosition = move->whiteMove ? board->blackKing : board->whiteKing;
    for ( uint index = 0; index < 64; ++index ) {
        IndexTranslation *position = lookup_translateIndex( index );
        Piece piece = board->pieceMap[position->row][position->col];
        if ( ( piece.isWhite && !move->whiteMove ) ||
             ( !piece.isWhite && move->whiteMove ) ) {
            continue;
        }
        moves = 0;
        switch ( piece.type ) {
            case NONE:
            case KING:
                break;
            case PAWN:
                moves = lookup_getPawnCaptures( index, move->whiteMove );
                break;
            case ROOK:
                if ( position->row != kingPosition.row &&
                     position->col != kingPosition.col ) {
                    break;
                }
                moves = getRookMoves( board, position->row, position->col );  
                break;
            case KNIGHT:
                moves = getKnightMoves( position->row, position->col );
                break;
            case BISHOP:
                if ( position->diaUpRight != kingPosition.diaUpRight &&
                     position->diaDownRight != kingPosition.diaDownRight ) {
                    break;
                }
                moves = getBishopMoves( board, position->row, position->col );  
                break;
            case QUEEN:
                if ( position->row != kingPosition.row &&
                     position->col != kingPosition.col  &&
                     position->diaUpRight != kingPosition.diaUpRight &&
                     position->diaDownRight != kingPosition.diaDownRight ) {
                    break;
                }
                moves = getQueenMoves( board, position->row, position->col );  
                break;
        }
        leadsToCheck = ( moves >> ( 63 - kingPosition.index ) & 1 );
        if ( leadsToCheck ) {
            break;
        }
    }
    //board_undoMove( board );
    memcpy( board->pieceMap, lastPieceMap, sizeof( Piece ) * 64 );
    board_updateBitFieldsFromPieces( board );
    board->whiteToMove = !board->whiteToMove;
    return leadsToCheck;
}

Move* board_getMovesForCurrentSide( Board *board, uint *numMoves ) {
    uint moveArraySize = 32;
    Move *moveArray = malloc( moveArraySize * sizeof( Move ) ); 
    *numMoves = 0;
    uint64_t moves, captures;
    for ( uint row = 0; row < 8; ++row ) {
        for ( uint col = 0; col < 8; ++col ) {
            captures = 0;
            if ( ( board->whiteToMove && !board->pieceMap[row][col].isWhite ) ||
                ( !board->whiteToMove && board->pieceMap[row][col].isWhite ) ) {
                continue;
            }
            moves = board_getMoveBitFieldForPiece( board, row, col);
            if ( board->pieceMap[row][col].type != PAWN ) {
                board_getCapturesFromMoves( board, &captures, &moves, board->pieceMap[row][col].isWhite );
            } else {
                board_addPawnCaptures( board, &captures, &moves, row, col );
            }

            bool enPassant;
            for ( uint i = 0; i < 64; ++i ) {
                enPassant = false;
                if ( !( moves >> ( 63 - i ) & 1 ) ) {
                    //en passant
                    if ( captures >> ( 63 - i ) & 1 ) {
                        enPassant = true;
                    } else {
                        continue;
                    }
                }
                if ( *numMoves == moveArraySize ) {
                    moveArraySize *= 2;
                    moveArray = realloc( moveArray, moveArraySize * sizeof( Move ) );
                }

                //check for promotion
                if ( i / 8 == ( board->whiteToMove ? 0 : 7 ) &&
                     ( board->pieceMap[row][col].type == PAWN ) ) {
                    for ( uint promotionPiece = KNIGHT; promotionPiece <= QUEEN; ++promotionPiece ) {
                        if ( *numMoves == moveArraySize ) {
                            moveArraySize *= 2;
                            moveArray = realloc( moveArray, moveArraySize * sizeof( Move ) );
                        }
                        moveArray[*numMoves].moveType = MOVE_PROMOTION;
                        moveArray[*numMoves].promotionType = promotionPiece;
                        moveArray[*numMoves].whiteMove = board->pieceMap[row][col].isWhite;
                        moveArray[*numMoves].pieceType = PAWN;
                        moveArray[*numMoves].srcRow = row;
                        moveArray[*numMoves].srcCol = col;
                        moveArray[*numMoves].dstRow = i / 8;
                        moveArray[*numMoves].dstCol = i % 8;
                        moveArray[*numMoves].leadsToCheck = board_moveLeadsToCheck( board, &moveArray[*numMoves] );
                        ++*numMoves;
                    }
                    continue;
                }
                moveArray[*numMoves].moveType = MOVE_NORMAL;
                moveArray[*numMoves].whiteMove = board->pieceMap[row][col].isWhite;
                moveArray[*numMoves].pieceType = board->pieceMap[row][col].type;
                moveArray[*numMoves].srcRow = row;
                moveArray[*numMoves].srcCol = col;
                moveArray[*numMoves].dstRow = i / 8;
                moveArray[*numMoves].dstCol = i % 8;

                if ( captures >> ( 63 - i ) & 1  ) {
                    moveArray[*numMoves].moveType = MOVE_CAPTURE;
                    moveArray[*numMoves].captureType = CAPTURE_NORMAL; //TODO: update to actually check for capture type
                    moveArray[*numMoves].pieceCaptured = board->pieceMap[i / 8][i % 8].type;
                    if ( enPassant ) {
                        moveArray[*numMoves].captureType = CAPTURE_EN_PASSANT;
                        moveArray[*numMoves].pieceCaptured = PAWN;
                    }
                }
                moveArray[*numMoves].leadsToCheck = board_moveLeadsToCheck( board, &moveArray[*numMoves] );

                //board_printMove( &moveArray[*numMoves] );

                ++*numMoves;
            }
            //printMoves( moves, row * 8 + col, symbol );
        }
    }
    if ( board_checkCastle( board, DIRECTION_LEFT ) ) {
        if ( *numMoves == moveArraySize ) {
            moveArraySize *= 2;
            moveArray = realloc( moveArray, moveArraySize * sizeof( Move ) );
        }
        moveArray[*numMoves].moveType = MOVE_CASTLE;
        moveArray[*numMoves].pieceType = KING;
        moveArray[*numMoves].castleDirection = DIRECTION_LEFT;
        moveArray[*numMoves].srcRow = board->whiteToMove ? 7 : 0;
        moveArray[*numMoves].srcCol = 4; //king col
        moveArray[*numMoves].dstRow = board->whiteToMove ? 7 : 0;
        moveArray[*numMoves].dstCol = 2;
        moveArray[*numMoves].leadsToCheck = board_moveLeadsToCheck( board, &moveArray[*numMoves] );
        ++*numMoves;
        
    }
    if ( board_checkCastle( board, DIRECTION_RIGHT ) ) {
        if ( *numMoves == moveArraySize ) {
            moveArraySize *= 2;
            moveArray = realloc( moveArray, moveArraySize * sizeof( Move ) );
        }
        moveArray[*numMoves].moveType = MOVE_CASTLE;
        moveArray[*numMoves].pieceType = KING;
        moveArray[*numMoves].castleDirection = DIRECTION_RIGHT;
        moveArray[*numMoves].srcRow = board->whiteToMove ? 7 : 0;
        moveArray[*numMoves].srcCol = 4; //king col
        moveArray[*numMoves].dstRow = board->whiteToMove ? 7 : 0;
        moveArray[*numMoves].dstCol = 6;
        moveArray[*numMoves].leadsToCheck = board_moveLeadsToCheck( board, &moveArray[*numMoves] );
        ++*numMoves;
        
    }
    return moveArray;
}

Move* board_getMovesForOppositeSide( Board *board, uint *numMoves ) {
    board->whiteToMove = !board->whiteToMove;
    Move *moveArray = board_getMovesForCurrentSide( board, numMoves );
    board->whiteToMove = !board->whiteToMove;
    return moveArray;
}

static int randomIndex( const uint size ) {
    if ( ( size - 1 ) == RAND_MAX ) {
        return rand();
    }
    // Supporting larger values for n would requires an even more
    // elaborate implementation that combines multiple calls to rand()
    // Chop off all of the values that would cause skew...
    int end = RAND_MAX / size; // truncate skew
    assert (end > 0);
    end *= size;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % size;
}

void board_decideAndMakeMove( Board *board ) {
    //get all of the moves of the currently moving side
    //do whatever algorithm to determine which move to do
    //make the move
    //update all the pieces within board
    //update all of the various bit fields within board


    uint numMoves = 0;
    Move *moves;
    /*
    board_updateBitFieldsFromPieces( board );
    moves = board_getMovesForCurrentSide( board, &numMoves );
    free( moves );
    return;
*/
    int random;
    for ( uint i = 0; i < 30; ++i ) {
        numMoves = 0;
        moves = board_getMovesForCurrentSide( board, &numMoves );
        random = randomIndex( numMoves );
        board_makeMove( board, &moves[random] );
        board_printMove( &moves[random] );
        board_print( board );
        free( moves );
    }
}

void board_playGame( Board *board ) {
    uint numMoves;
    Move *moves;
    while ( true ) {
        numMoves = 0;
        board_print( board );
        printf( "%s turn:\n", board->whiteToMove ? "White's" : "Black's" );
        moves = board_getMovesForCurrentSide( board, &numMoves );
        for ( uint i = 0; i < numMoves; ++i ) {
            printf( "%u: ", i );
            board_printMove( &moves[i] );
        }
        uint index = numMoves;
        char buffer[5];
        while ( index >= numMoves ) {
            printf( "\nChoose an index from 0-%u: ", numMoves - 1 );
            fgets( buffer, 5, stdin );
            index = atoi( buffer );
        }
        board_printMove( &moves[index] );
        printf( "White king pos: (%u, %u)\n", board->whiteKing.row, board->whiteKing.col );
        printf( "Black king pos: (%u, %u)\n", board->blackKing.row, board->blackKing.col );
        board_makeMove( board, &moves[index] );
        free( moves );
    }
}
