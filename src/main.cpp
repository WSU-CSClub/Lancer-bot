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


// Test positions
std::string temp_fen = "r1bq1k1r/p3bppp/1pn1pn2/2p5/2B1NB2/3P1N2/PPP2PPP/R2QR1K1 w - - 0 11";
std::string opening_fen1 = "r1bqkbnr/pppp1ppp/2n5/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 3 3";
std::string opening_fen2 = "rnbqkb1r/pp3ppp/2p1pn2/3p4/2PP4/2N2N2/PP2PPPP/R1BQKB1R w KQkq - 0 5";
std::string mid_fen1 = "r1bqk2r/ppp2ppp/3p1n2/n1b1p3/N1B1P3/3P1N2/PPP2PPP/R1BQK2R w KQkq - 2 7";
std::string mid_fen2 = "rnbqkb1r/p4p2/2p1pn1p/1p4p1/P1pPP3/2N2NB1/1P3PPP/R2QKB1R b KQkq - 0 9";
std::string end_fen1 = "8/1k3p2/pp4p1/3Pp3/6P1/PK6/7P/8 w - - 0 2";
std::string end_fen2 = "3k4/8/4PK2/8/8/8/8/8 w - - 1 5";


int runPositions(ChessBoard board) {
    // Display the current board state
    printBoard(board);

    // Initialize move generator
    MoveGen moveGen(board);

    // Generate and print moves for both sides
    std::cout << "\nLegal moves from this position:\n";

    // White moves
    std::vector<Move> whiteMoves = moveGen.GenerateMoves(true);
    std::cout << "\nWhite moves (" << whiteMoves.size() << " moves):\n";
    for (const Move& move : whiteMoves) {
        printMove(move);
    }

    // Black moves
    std::vector<Move> blackMoves = moveGen.GenerateMoves(false);
    std::cout << "\n\nBlack moves (" << blackMoves.size() << " moves):\n";
    for (const Move& move : blackMoves) {
        printMove(move);
    }
    std::cout << "\n";

    Evaluation evaluator(board, moveGen);

    MinimaxSearch minimaxSearch(board, moveGen, evaluator);
    std::cout << "\nCalculating best move...\n";
    Move bestMove = minimaxSearch.findBestMove(false);
    std::cout << "Best move found: ";
    printMove(bestMove);
    std::cout << "\n";

    std::cout << "score : " << evaluator.evaluate(true) << std::endl;

    return 0;
}

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
        
        // Unit test positions
        setPositionFromFEN(board, opening_fen1);
        runPositions(board);                        // Best move should either be Nf6 or Bc5, eval +.2
        setPositionFromFEN(board, opening_fen2);
        runPositions(board);                        // Best move should either be e3 or Bg5, eval +.2
        setPositionFromFEN(board, mid_fen1);
        runPositions(board);                        // Best move should be Nxc5, eval +1.3
        setPositionFromFEN(board, mid_fen2);
        runPositions(board);                        // Best move should be b4, eval -.6
        setPositionFromFEN(board, end_fen1);
        runPositions(board);                        // Best move should be h4, eval +infinity
        setPositionFromFEN(board, end_fen2);
        runPositions(board);                        // Best move is Kf7, eval +infinity

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}