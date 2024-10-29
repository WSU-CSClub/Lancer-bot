#ifndef BOARD_HPP
#define BOARD_HPP

#include <cstdint>
#include <vector>


// Add Pieces to the ENUM
enum Piece { WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK};

using Bitboard = uint64_t;


// TODO: check if vector > array for this case
//        as it's a constant size collection



//BitBoard
using ChessBoard = std::vector<Bitboard>;

void setPiece(Bitboard &b, int file, int rank);

void printBoard(const ChessBoard &pieceBitboards);

void initBoard(ChessBoard &b);

#endif
