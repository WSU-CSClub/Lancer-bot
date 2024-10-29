#include "engine/board.hpp"
#include <cstdio>

int main() {
  // Example bitboard
  ChessBoard board(12, 0);
  initBoard(board);

  // Print the board
  printBoard(board);

  return 0;
}
