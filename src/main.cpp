#include "engine/board.hpp"
#include "engine/movegen.hpp"
#include <cstdio>


void printMove(const Move& move) {
    // Convert square numbers to chess notation
    char fromFile = 'a' + (move.from % 8);
    int fromRank = 1 + (move.from / 8);
    char toFile = 'a' + (move.to % 8);
    int toRank = 1 + (move.to / 8);
    
    printf("%c%d%c%d ", fromFile, fromRank, toFile, toRank);
}


int main() {
  // Example bitboard
  ChessBoard board(12, 0);
  initBoard(board);

  // Print the board
  printBoard(board);


MoveGen moveGen(board);



    // Generate moves for white
    std::vector<Move> whiteMoves = moveGen.GenerateMoves(true);
    
    // Print all white moves
    printf("\nWhite moves:\n");
    for(const Move& move : whiteMoves) {
        printMove(move);
    }
    
    // Generate moves for black
    std::vector<Move> blackMoves = moveGen.GenerateMoves(false);
    
    // Print all black moves
    printf("\n\nBlack moves:\n");
    for(const Move& move : blackMoves) {
        printMove(move);
    }
  return 0;
}
