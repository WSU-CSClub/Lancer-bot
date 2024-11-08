#pragma once
#include <iostream>
#include "../engine/movegen.hpp"
   uint64_t getFileMask(int square) {
        return 0x0101010101010101ULL << (square & 7);
    }

class Evaluation {
private:
    // Piece values in centipawns (100 = 1 pawn)
    static constexpr int PAWN_VALUE = 100;
    static constexpr int KNIGHT_VALUE = 320;
    static constexpr int BISHOP_VALUE = 330;
    static constexpr int ROOK_VALUE = 500;
    static constexpr int QUEEN_VALUE = 900;
    static constexpr int KING_VALUE = 20000;

    // Piece-Square Tables for middlegame
    // Tables are from White's perspective - need to flip for Black
    static constexpr int PAWN_PST[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    static constexpr int KNIGHT_PST[64] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };

    static constexpr int BISHOP_PST[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };

    ChessBoard& board;

    // Helper function to count bits (pieces) in a bitboard
    int countPieces(uint64_t bitboard) {
        int count = 0;
        while (bitboard) {
            count++;
            bitboard &= (bitboard - 1);
        }
        return count;
    }

    // Helper to get position value from PST (for white pieces)
    int getPSTValue(int square, const int table[64]) {
        return table[square];
    }

    // Helper to get position value from PST (for black pieces - needs to flip square)
    int getBlackPSTValue(int square, const int table[64]) {
        return table[63 - square];
    }

    // Evaluate pawn structure
    int evaluatePawnStructure(bool isWhite) {
        int score = 0;
        uint64_t pawns = isWhite ? board[WP] : board[BP];
        
        // Evaluate each pawn
        while (pawns) {
            int square = getLSB(pawns);
            
            // Basic piece-square table evaluation
            score += isWhite ? 
                    getPSTValue(square, PAWN_PST) :
                    getBlackPSTValue(square, PAWN_PST);

            // Penalize doubled pawns
            uint64_t file_mask = getFileMask(square);
            int pawnsOnFile = countPieces(pawns & file_mask);
            if (pawnsOnFile > 1) {
                score -= 20 * (pawnsOnFile - 1);
            }

            // Bonus for passed pawns
            uint64_t ahead_mask = isWhite ? 
                (file_mask << 8) & (-(1ULL << square)) :
                (file_mask >> 8) & ((1ULL << square) - 1);
            
            if (!(ahead_mask & (isWhite ? board[BP] : board[WP]))) {
                score += 50; // Passed pawn bonus
            }

            pawns &= pawns - 1;
        }
        
        return score;
    }

public:
    Evaluation(ChessBoard& b) : board(b) {}

    int evaluate() {
        int score = 0;

        // Material counting
        score += PAWN_VALUE * (countPieces(board[WP]) - countPieces(board[BP]));
        score += KNIGHT_VALUE * (countPieces(board[WN]) - countPieces(board[BN]));
        score += BISHOP_VALUE * (countPieces(board[WB]) - countPieces(board[BB]));
        score += ROOK_VALUE * (countPieces(board[WR]) - countPieces(board[BR]));
        score += QUEEN_VALUE * (countPieces(board[WQ]) - countPieces(board[BQ]));

        // Piece-square table evaluation
        uint64_t pieces = board[WN];
        while (pieces) {
            int square = getLSB(pieces);
            score += getPSTValue(square, KNIGHT_PST);
            pieces &= pieces - 1;
        }

        pieces = board[BN];
        while (pieces) {
            int square = getLSB(pieces);
            score -= getBlackPSTValue(square, KNIGHT_PST);
            pieces &= pieces - 1;
        }

        pieces = board[WB];
        while (pieces) {
            int square = getLSB(pieces);
            score += getPSTValue(square, BISHOP_PST);
            pieces &= pieces - 1;
        }

        pieces = board[BB];
        while (pieces) {
            int square = getLSB(pieces);
            score -= getBlackPSTValue(square, BISHOP_PST);
            pieces &= pieces - 1;
        }

        // Pawn structure evaluation
        score += evaluatePawnStructure(true);   // White pawns
        score -= evaluatePawnStructure(false);  // Black pawns

        // Bishop pair bonus
        if (countPieces(board[WB]) >= 2) score += 50;
        if (countPieces(board[BB]) >= 2) score -= 50;

        // Return score from White's perspective
        return score;
    }
};