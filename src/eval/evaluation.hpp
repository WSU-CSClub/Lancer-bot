#pragma once
#include <iostream>
#include "../engine/movegen.hpp"

class Evaluation {
private:
    // Piece values in centipawns (100 = 1 pawn)
    static constexpr int PAWN_VALUE = 100;
    static constexpr int KNIGHT_VALUE = 320;
    static constexpr int BISHOP_VALUE = 330;
    static constexpr int ROOK_VALUE = 500;
    static constexpr int QUEEN_VALUE = 900;
    static constexpr int KING_VALUE = 20000;

    // Piece mobility bonuses
    static constexpr int KNIGHT_MOBILITY_BONUS = 4;
    static constexpr int BISHOP_MOBILITY_BONUS = 3;
    static constexpr int ROOK_MOBILITY_BONUS = 2;
    static constexpr int QUEEN_MOBILITY_BONUS = 1;

    // King safety penalties
    static constexpr int KING_SHIELD_BONUS = 10;
    static constexpr int KING_OPEN_FILE_PENALTY = -30;

    // Pawn structure values
    static constexpr int DOUBLED_PAWN_PENALTY = -20;
    static constexpr int ISOLATED_PAWN_PENALTY = -20;
    static constexpr int PASSED_PAWN_BONUS = 50;
    static constexpr int PROTECTED_PASSED_PAWN_BONUS = 70;
    static constexpr int CONNECTED_PAWNS_BONUS = 10;

    // Piece coordination bonuses
    static constexpr int ROOK_ON_OPEN_FILE_BONUS = 30;
    static constexpr int ROOK_ON_SEMI_OPEN_FILE_BONUS = 15;
    static constexpr int ROOK_CONNECTED_BONUS = 20;
    static constexpr int BISHOP_PAIR_BONUS = 50;
    static constexpr int KNIGHT_OUTPOST_BONUS = 30;

    static constexpr uint64_t FILE_A = 0x0101010101010101ULL;
    static constexpr uint64_t FILE_H = 0x8080808080808080ULL;
    
    // Central squares (e4, e5, d4, d5)
    static constexpr uint64_t CENTRAL_SQUARES = 
        (1ULL << 27) | (1ULL << 28) | (1ULL << 35) | (1ULL << 36);

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
    MoveGen& moveGen;

    uint64_t getFileMask(int square) {
        return 0x0101010101010101ULL << (square & 7);
    }

    int countPieces(uint64_t bitboard) {
        int count = 0;
        while (bitboard) {
            count++;
            bitboard &= (bitboard - 1);
        }
        return count;
    }

    int getPSTValue(int square, const int table[64]) {
        return table[square];
    }

    int getBlackPSTValue(int square, const int table[64]) {
        return table[63 - square];
    }
    
    // Enhanced pawn structure evaluation
    int evaluatePawnStructure(bool isWhite) {
        int score = 0;
        uint64_t pawns = isWhite ? board[WP] : board[BP];
        uint64_t enemyPawns = isWhite ? board[BP] : board[WP];
        
        while (pawns) {
            int square = getLSB(pawns);
            int file = square & 7;
            
            // Basic PST score
            score += isWhite ? 
                    getPSTValue(square, PAWN_PST) :
                    getBlackPSTValue(square, PAWN_PST);

            uint64_t fileMask = getFileMask(square);
            
            // Doubled pawns
            int pawnsOnFile = countPieces(pawns & fileMask);
            if (pawnsOnFile > 1) {
                score += DOUBLED_PAWN_PENALTY * (pawnsOnFile - 1);
            }

            // Isolated pawns
            uint64_t adjacentFiles = 0;
            if (file > 0) adjacentFiles |= getFileMask(square - 1);
            if (file < 7) adjacentFiles |= getFileMask(square + 1);
            if (!(pawns & adjacentFiles)) {
                score += ISOLATED_PAWN_PENALTY;
            }

            // Passed pawns
            uint64_t frontSpan = isWhite ? 
                (fileMask << 8) & (-(1ULL << square)) :
                (fileMask >> 8) & ((1ULL << square) - 1);
            uint64_t adjacentFrontSpan = 0;
            if (file > 0) adjacentFrontSpan |= frontSpan >> 1;
            if (file < 7) adjacentFrontSpan |= frontSpan << 1;
            
            if (!(frontSpan & enemyPawns) && !(adjacentFrontSpan & enemyPawns)) {
                score += PASSED_PAWN_BONUS;
                
                // Get all moves for this pawn to check if it's protected
                auto pawnMoves = moveGen.GenerateMoves(isWhite);
                bool isProtected = false;
                for (const auto& move : pawnMoves) {
                    if (move.to == square && (board[WP] & (1ULL << move.from))) {
                        isProtected = true;
                        break;
                    }
                }
                if (isProtected) {
                    score += PROTECTED_PASSED_PAWN_BONUS;
                }
            }

            pawns &= pawns - 1;
        }
        
        return score;
    }

    // Evaluate king safety using MoveGen
    int evaluateKingSafety(bool isWhite) {
        int score = 0;
        int kingSquare = getLSB(isWhite ? board[WK] : board[BK]);
        int kingFile = kingSquare & 7;

        uint64_t friendlyPawns = isWhite ? board[WP] : board[BP];
        
        // Get king moves to determine king zone
        auto kingMoves = moveGen.GenerateMoves(isWhite);
        uint64_t kingZone = 0ULL;
        for (const auto& move : kingMoves) {
            if ((1ULL << move.from) & (isWhite ? board[WK] : board[BK])) {
                kingZone |= (1ULL << move.to);
            }
        }

        // Pawn shield
        uint64_t pawnShield = isWhite ?
            (kingZone >> 8) & friendlyPawns :
            (kingZone << 8) & friendlyPawns;
        score += countPieces(pawnShield) * KING_SHIELD_BONUS;

        // Open files near king
        for (int f = std::max(0, kingFile - 1); f <= std::min(7, kingFile + 1); f++) {
            uint64_t fileMask = getFileMask(f);
            if (!(fileMask & (board[WP] | board[BP]))) {
                score += KING_OPEN_FILE_PENALTY;
            }
        }

        return score;
    }

    // Evaluate piece coordination using MoveGen
    int evaluatePieceCoordination(bool isWhite) {
        int score = 0;
        
        // Rook evaluation
        uint64_t rooks = isWhite ? board[WR] : board[BR];
        uint64_t allPawns = board[WP] | board[BP];
        
        while (rooks) {
            int square = getLSB(rooks);
            uint64_t fileMask = getFileMask(square);
            
            // Rook on open file
            if (!(fileMask & allPawns)) {
                score += ROOK_ON_OPEN_FILE_BONUS;
            }
            // Rook on semi-open file
            else if (!(fileMask & (isWhite ? board[WP] : board[BP]))) {
                score += ROOK_ON_SEMI_OPEN_FILE_BONUS;
            }
            
            // Get rook moves to check connectivity
            auto rookMoves = moveGen.GenerateMoves(isWhite);
            for (const auto& move : rookMoves) {
                if ((1ULL << move.from) & rooks && (1ULL << move.to) & rooks) {
                    score += ROOK_CONNECTED_BONUS;
                    break;
                }
            }
            
            rooks &= rooks - 1;
        }

        // Bishop pair bonus
        if (countPieces(isWhite ? board[WB] : board[BB]) >= 2) {
            score += BISHOP_PAIR_BONUS;
        }

        // Knight outposts
        uint64_t knights = isWhite ? board[WN] : board[BN];
        uint64_t enemyPawnMoves = 0ULL;
        
        // Get enemy pawn attacks using MoveGen
        auto enemyPawnMovesList = moveGen.GenerateMoves(!isWhite);
        for (const auto& move : enemyPawnMovesList) {
            if ((board[isWhite ? BP : WP] & (1ULL << move.from))) {
                enemyPawnMoves |= (1ULL << move.to);
            }
        }
        
            score += evaluateKnightOutposts(isWhite);

          return score;
    }

    // Evaluate mobility using MoveGen
    int evaluateMobility(bool isWhite) {
        auto moves = moveGen.GenerateMoves(isWhite);
        int score = 0;
        
        for (const auto& move : moves) {
            uint64_t fromBit = 1ULL << move.from;
            
            // Assign mobility bonus based on piece type
            if (fromBit & board[isWhite ? WN : BN]) score += KNIGHT_MOBILITY_BONUS;
            else if (fromBit & board[isWhite ? WB : BB]) score += BISHOP_MOBILITY_BONUS;
            else if (fromBit & board[isWhite ? WR : BR]) score += ROOK_MOBILITY_BONUS;
            else if (fromBit & board[isWhite ? WQ : BQ]) score += QUEEN_MOBILITY_BONUS;
        }
        
        return score;
    }
int evaluateKnightOutposts(bool isWhite) {
    int score = 0;
    uint64_t knights = isWhite ? board[WN] : board[BN];
    uint64_t friendlyPawns = isWhite ? board[WP] : board[BP];
    
    // Get squares attacked by enemy pawns
    uint64_t enemyPawnMoves = 0ULL;
    auto enemyPawnMovesList = moveGen.GenerateMoves(!isWhite);
    for (const auto& move : enemyPawnMovesList) {
        if ((board[isWhite ? BP : WP] & (1ULL << move.from))) {
            enemyPawnMoves |= (1ULL << move.to);
        }
    }

    // Get squares controlled by equal/lesser value pieces
    uint64_t enemyControl = 0ULL;
    auto enemyMoves = moveGen.GenerateMoves(!isWhite);
    for (const auto& move : enemyMoves) {
        uint64_t fromBit = 1ULL << move.from;
        // Only consider knights and bishops (equal or lesser value)
        if ((fromBit & board[isWhite ? BN : WN]) || 
            (fromBit & board[isWhite ? BB : WB])) {
            enemyControl |= (1ULL << move.to);
        }
    }

    while (knights) {
        int square = getLSB(knights);
        uint64_t squareBit = 1ULL << square;
        
        // Condition 1: Protected by friendly pawn
        bool pawnProtected = false;
        uint64_t pawnAttacks = isWhite ?
            ((squareBit >> 7) & ~FILE_H) | ((squareBit >> 9) & ~FILE_A) :  // White pawn attacks
            ((squareBit << 7) & ~FILE_A) | ((squareBit << 9) & ~FILE_H);   // Black pawn attacks
        
        if (pawnAttacks & friendlyPawns) {
            pawnProtected = true;
        }

        // Condition 2: Cannot be attacked by enemy pawns
        bool safeFromPawns = !(enemyPawnMoves & squareBit);

        // Condition 3: Not easily challenged by equal value pieces
        bool notEasilyChallenged = !(enemyControl & squareBit);

        // Calculate rank bonus (outposts are stronger in enemy territory)
        int rank = square >> 3;
        int rankBonus = isWhite ? 
            std::max(0, rank - 3) * 5 :  // Bonus for ranks 4-7 for white
            std::max(0, 4 - rank) * 5;   // Bonus for ranks 3-0 for black

        // Only give outpost bonus if all conditions are met
        if (pawnProtected && safeFromPawns && notEasilyChallenged) {
            score += KNIGHT_OUTPOST_BONUS + rankBonus;
            
            // Additional bonus for central control
            int file = square & 7;
            if ((rank >= 3 && rank <= 4) && (file >= 2 && file <= 5)) {
                score += 10;  // Bonus for controlling center from outpost
            }
        }

        knights &= knights - 1;
    }
    
    return score;
}

    bool isWhiteTurn(const std::string& fen) {
        // Find the first space
        size_t spacePos = fen.find(' ');
        if (spacePos != std::string::npos && spacePos + 1 < fen.length()) {
            // Next character after space indicates turn
            return fen[spacePos + 1] == 'w';
        }
        return true; // Default to white if FEN is malformed
    }



public:
    Evaluation(ChessBoard& b, MoveGen& mg) : board(b), moveGen(mg) {}

double normalizeScore(int score) {
    // Define practical centipawn thresholds for Stockfish-like behavior
    const int MAX_CENTIPAWN = 100; // Maximum meaningful centipawn score for evaluation
    const int MATE_THRESHOLD = 100000; // Scores above this are considered mate-related

    // Forced mate handling
    if (score > MATE_THRESHOLD) return 1.0;    // White is guaranteed to win (mate for White)
    if (score < -MATE_THRESHOLD) return -1.0;  // Black is guaranteed to win (mate for Black)

    // Normalize centipawn scores into a range of [-1, 1]
    return static_cast<double>(score) / MAX_CENTIPAWN;
}

    double evaluate(bool isWhiteTurn) {
        int score = 0;

        // Material counting
        score += PAWN_VALUE * (countPieces(board[WP]) - countPieces(board[BP]));
        score += KNIGHT_VALUE * (countPieces(board[WN]) - countPieces(board[BN]));
        score += BISHOP_VALUE * (countPieces(board[WB]) - countPieces(board[BB]));
        score += ROOK_VALUE * (countPieces(board[WR]) - countPieces(board[BR]));
        score += QUEEN_VALUE * (countPieces(board[WQ]) - countPieces(board[BQ]));

        // Positional evaluation
        score += evaluatePawnStructure(true) - evaluatePawnStructure(false);
        score += evaluateKingSafety(true) - evaluateKingSafety(false);
        score += evaluatePieceCoordination(true) - evaluatePieceCoordination(false);

        // Mobility evaluation
        score += evaluateMobility(true) - evaluateMobility(false);

        // Tempo bonus: having the move is worth about 10 centipawns
        // Add when i have time or when im not lazy :0
        const int TEMPO_BONUS = 10;
        // score += isWhiteTurn ? TEMPO_BONUS : -TEMPO_BONUS;

        // If it's black's turn, negate the score
        // This is because the score is always from the perspective of the side to move
        double normalizedScore = normalizeScore(score);
        return isWhiteTurn ? normalizedScore : -normalizedScore;
    }
};