#include "board.hpp"
#include <iostream>
#include <vector>

void setPiece(Bitboard &bitboard, int file, int rank) {
  int square = rank * 8 + file;
  bitboard |= (1ULL << square);
  std::cout << "Setting piece at file " << file << ", rank " << rank
            << " (square " << square << ")" << std::endl;
}

void initBoard(ChessBoard &b) {
  // Set up pieces for White
  setPiece(b[WR], 0, 0); // White Rook on a1
  setPiece(b[WN], 1, 0); // White Knight on b1
  setPiece(b[WB], 2, 0); // White Bishop on c1
  setPiece(b[WQ], 3, 0); // White Queen on d1
  setPiece(b[WK], 4, 0); // White King on e1
  setPiece(b[WB], 5, 0); // White Bishop on f1
  setPiece(b[WN], 6, 0); // White Knight on g1
  setPiece(b[WR], 7, 0); // White Rook on h1
  for (int file = 0; file < 8; ++file) {
    setPiece(b[WP], file, 1); // White Pawns on rank 2
  }

  // Set up pieces for Black
  setPiece(b[BR], 0, 7); // Black Rook on a8
  setPiece(b[BN], 1, 7); // Black Knight on b8
  setPiece(b[BB], 2, 7); // Black Bishop on c8
  setPiece(b[BQ], 3, 7); // Black Queen on d8
  setPiece(b[BK], 4, 7); // Black King on e8
  setPiece(b[BB], 5, 7); // Black Bishop on f8
  setPiece(b[BN], 6, 7); // Black Knight on g8
  setPiece(b[BR], 7, 7); // Black Rook on h8
  for (int file = 0; file < 8; ++file) {
    setPiece(b[BP], file, 6); // Black Pawns on rank 7
  }
}

void printBoard(const ChessBoard &pieceBitboards) {
  char board[64];

  // Fill board with empty squares
  std::fill(std::begin(board), std::end(board), '.');

  // Map each square to the appropriate piece character
  for (int i = 0; i < pieceBitboards.size(); ++i) {
    Bitboard bitboard = pieceBitboards[i];

    for (int square = 0; square < 64; ++square) {
      if (!((bitboard >> square) & 1ULL)) {
        continue;
      }

      switch (i) {
      case WP:
        board[square] = 'P';
        break;
      case WN:
        board[square] = 'N';
        break;
      case WB:
        board[square] = 'B';
        break;
      case WR:
        board[square] = 'R';
        break;
      case WQ:
        board[square] = 'Q';
        break;
      case WK:
        board[square] = 'K';
        break;
      case BP:
        board[square] = 'p';
        break;
      case BN:
        board[square] = 'n';
        break;
      case BB:
        board[square] = 'b';
        break;
      case BR:
        board[square] = 'r';
        break;
      case BQ:
        board[square] = 'q';
        break;
      case BK:
        board[square] = 'k';
        break;
      }
    }
  }

  // Print the board
  std::cout << "\nChess Board:\n";
  for (int rank = 7; rank >= 0; --rank) {
    std::cout << rank + 1 << " "; // Rank label
    for (int file = 0; file < 8; ++file) {
      std::cout << board[rank * 8 + file] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "  a b c d e f g h\n"; // File labels
}
