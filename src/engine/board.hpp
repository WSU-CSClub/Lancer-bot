#ifndef BOARD_HPP
#define BOARD_HPP

#include <cstdint>
#include <vector>
#include <string>


// Add Pieces to the ENUM
enum Piece { WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK};

using Bitboard = uint64_t;


//BitBoard
using ChessBoard = std::vector<Bitboard>;

void setPiece(Bitboard &b, int file, int rank);

void printBoard(const ChessBoard &pieceBitboards);

void initBoard(ChessBoard &b);

void setPositionFromFEN(ChessBoard &board, const std::string &fen);

bool isWhiteturnFen(const std::string& fen);


#endif
