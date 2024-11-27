#include "engine/board.hpp"
#include "engine/movegen.hpp"
#include "network/network.hpp"
#include <cstdio>
#include <iostream>
#include "eval/evaluation.hpp"
#include "engine/search.hpp"
void printMove(const Move& move) {
    char fromFile = 'a' + (move.from % 8);
    int fromRank = 1 + (move.from / 8);
    char toFile = 'a' + (move.to % 8);
    int toRank = 1 + (move.to / 8);
    
    printf("%c%d%c%d ", fromFile, fromRank, toFile, toRank);
}

std::string temp_fen = "r1bq1k1r/p3bppp/1pn1pn2/2p5/2B1NB2/3P1N2/PPP2PPP/R2QR1K1 w - - 0 11";

int main() {
    try {
        // Initialize database connection
        ChessEngineDB db("database/chess_openings.db");
        
        // Load and display Italian opening position
        std::cout << "Loading Italian Opening Position:\n";
        auto italian = db.getItalianPosition();
        
        // Print FEN notation for each row
        for (const auto& pos : italian) {
            std::cout << "Row " << pos.row << ": " << pos.fen_notation << "\n";
        }
        
        // Get complete FEN string
        std::string complete_fen = db.getCompleteFEN(italian);
        std::cout << "\nComplete FEN: " << complete_fen << "\n\n";

        // Initialize board
        ChessBoard board(12, 0);
        
        // Load either starting position or Italian position
        bool useItalianOpening = true;  // Toggle between positions
        
        if (useItalianOpening) {
            // Set Italian opening position
            setPositionFromFEN(board, complete_fen);
            std::cout << "Loaded Italian Opening Position:\n";
        } else {
            // Set starting position
            setPositionFromFEN(board,temp_fen);
            std::cout << "Loaded Starting Position:\n";
        }
        

        // Display the current board state
        printBoard(board);
        
        // Initialize move generator
        MoveGen moveGen(board);
        
        // Generate and print moves for both sides
        std::cout << "\nLegal moves from this position:\n";
        
        // White moves
        std::vector<Move> whiteMoves = moveGen.GenerateMoves(true);
        std::cout << "\nWhite moves (" << whiteMoves.size() << " moves):\n";
        for(const Move& move : whiteMoves) {
            printMove(move);
        }
        
        // Black moves
        std::vector<Move> blackMoves = moveGen.GenerateMoves(false);
        std::cout << "\n\nBlack moves (" << blackMoves.size() << " moves):\n";
        for(const Move& move : blackMoves) {
            printMove(move);
        }
        std::cout << "\n";
        
        Evaluation evaluator(board,moveGen);
        auto score = evaluator.evaluate(isWhiteturnFen(complete_fen));

        MinimaxSearch minimaxSearch(board, moveGen, evaluator);
        std::cout << "\nCalculating best move...\n";
        Move bestMove = minimaxSearch.findBestMove(false);
        std::cout << "Best move found: ";
        printMove(bestMove);
        std::cout << "\n";

        std::cout << "score : " << score << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}