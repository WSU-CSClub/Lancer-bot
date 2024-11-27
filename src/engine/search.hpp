#pragma once
#include <algorithm>
#include <limits>
#include "board.hpp"
#include "movegen.hpp"
#include "../eval/evaluation.hpp"

class MinimaxSearch {
private:
    ChessBoard& board;
    MoveGen& moveGen;
    Evaluation& evaluator;
    static constexpr int MAX_DEPTH = 100;  // Adjust based on desired search depth

    // Helper function to make a move on the board
    void makeMove(const Move& move, bool isWhite) {
        uint64_t fromBit = 1ULL << move.from;
        uint64_t toBit = 1ULL << move.to;
        
        // Find which piece is moving
        for (int piece = 0; piece < 12; piece++) {
            if (board[piece] & fromBit) {
                // Remove piece from source square
                board[piece] &= ~fromBit;
                // Add piece to destination square
                board[piece] |= toBit;
                break;
            }
        }
        
        // Handle captures
        uint64_t opposingPieces = isWhite ? 
            (board[BP] | board[BN] | board[BB] | board[BR] | board[BQ] | board[BK]) :
            (board[WP] | board[WN] | board[WB] | board[WR] | board[WQ] | board[WK]);
            
        if (toBit & opposingPieces) {
            // Remove captured piece
            for (int piece = 0; piece < 12; piece++) {
                if (board[piece] & toBit) {
                    board[piece] &= ~toBit;
                    break;
                }
            }
        }
    }

    // Helper function to unmake a move
    void unmakeMove(const Move& move, bool isWhite, uint64_t capturedPiece = 0) {
        uint64_t fromBit = 1ULL << move.from;
        uint64_t toBit = 1ULL << move.to;
        
        // Find which piece is moving
        for (int piece = 0; piece < 12; piece++) {
            if (board[piece] & toBit) {
                // Remove piece from destination square
                board[piece] &= ~toBit;
                // Add piece back to source square
                board[piece] |= fromBit;
                break;
            }
        }
        
        // Restore captured piece if any
        if (capturedPiece) {
            for (int piece = 0; piece < 12; piece++) {
                if (capturedPiece & (1ULL << piece)) {
                    board[piece] |= toBit;
                    break;
                }
            }
        }
    }

    double minimax(int depth, bool isWhite, double alpha, double beta) {
        if (depth == 0) {
            return evaluator.evaluate(isWhite);
        }

        std::vector<Move> moves = moveGen.GenerateMoves(isWhite);
        if (moves.empty()) {
            // If no moves are available, this might be checkmate or stalemate
            return isWhite ? -1.0 : 1.0;  // Return worst score for the current player
        }

        double bestValue = isWhite ? -std::numeric_limits<double>::infinity() 
                                 : std::numeric_limits<double>::infinity();

        for (const Move& move : moves) {
            // Store state before move
            uint64_t capturedPiece = 0;
            for (int piece = 0; piece < 12; piece++) {
                if (board[piece] & (1ULL << move.to)) {
                    capturedPiece = board[piece] & (1ULL << move.to);
                    break;
                }
            }

            // Make move
            makeMove(move, isWhite);
            
            // Recursive evaluation
            double value = minimax(depth - 1, !isWhite, alpha, beta);
            
            // Unmake move
            unmakeMove(move, isWhite, capturedPiece);

            // Update best value
            if (isWhite) {
                bestValue = std::max(bestValue, value);
                alpha = std::max(alpha, bestValue);
            } else {
                bestValue = std::min(bestValue, value);
                beta = std::min(beta, bestValue);
            }

            // Alpha-beta pruning
            if (beta <= alpha) {
                break;
            }
        }

        return bestValue;
    }

public:
    MinimaxSearch(ChessBoard& b, MoveGen& mg, Evaluation& eval) 
        : board(b), moveGen(mg), evaluator(eval) {}

    Move findBestMove(bool isWhite) {
        std::vector<Move> moves = moveGen.GenerateMoves(isWhite);
        if (moves.empty()) {
            throw std::runtime_error("No moves available");
        }

        Move bestMove = moves[0];
        double bestValue = isWhite ? -std::numeric_limits<double>::infinity() 
                                 : std::numeric_limits<double>::infinity();
        double alpha = -std::numeric_limits<double>::infinity();
        double beta = std::numeric_limits<double>::infinity();

        for (const Move& move : moves) {
            // Store state before move
            uint64_t capturedPiece = 0;
            for (int piece = 0; piece < 12; piece++) {
                if (board[piece] & (1ULL << move.to)) {
                    capturedPiece = board[piece] & (1ULL << move.to);
                    break;
                }
            }

            // Make move
            makeMove(move, isWhite);
            
            // Evaluate position after move
            double value = minimax(MAX_DEPTH - 1, !isWhite, alpha, beta);
            
            // Unmake move
            unmakeMove(move, isWhite, capturedPiece);

            // Update best move if necessary
            if (isWhite) {
                if (value > bestValue) {
                    bestValue = value;
                    bestMove = move;
                }
                alpha = std::max(alpha, bestValue);
            } else {
                if (value < bestValue) {
                    bestValue = value;
                    bestMove = move;
                }
                beta = std::min(beta, bestValue);
            }
        }

        return bestMove;
    }
};