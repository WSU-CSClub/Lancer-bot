#include "engine/board.hpp"
#include "engine/movegen.hpp"
#include "network/network.hpp"
#include <cstdio>
#include <iostream>


void printMove(const Move& move) {
    // Convert square numbers to chess notation
    char fromFile = 'a' + (move.from % 8);
    int fromRank = 1 + (move.from / 8);
    char toFile = 'a' + (move.to % 8);
    int toRank = 1 + (move.to / 8);
    
    printf("%c%d%c%d ", fromFile, fromRank, toFile, toRank);
}


int main() {
  try{
        ChessEngineDB db("database/chess_openings.db");
        std::cout << "Italian Opening Position:\n";
        auto italian = db.getItalianPosition();
        
        // Print each row of the board
        for (const auto& pos : italian) {
            std::cout << "Row " << pos.row << ": " << pos.fen_notation << "\n";
        }
        std::string complete_fen = db.getCompleteFEN(italian);
        std::cout << "\nComplete FEN: " << complete_fen << "\n\n";

  }
  catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

  // Example bitboard
  ChessBoard board(12, 0);
  initBoard(board);

  // Print the board
  // printBoard(board);


MoveGen moveGen(board);



    // Generate moves for white
    std::vector<Move> whiteMoves = moveGen.GenerateMoves(true);
    
    // Print all white moves
    // printf("\nWhite moves:\n");
    for(const Move& move : whiteMoves) {
        // printMove(move);
    }
    
    // Generate moves for black
    std::vector<Move> blackMoves = moveGen.GenerateMoves(false);
    
    // Print all black moves
    // printf("\n\nBlack moves:\n");
    for(const Move& move : blackMoves) {
        // printMove(move);
    }
  return 0;
}
