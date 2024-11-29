#pragma once 
#include <iostream>
#include "board.hpp"
#include <array>

// For  windows users!
inline int getLSB(uint64_t b) {
#if defined(_MSC_VER)  // If using MSVC
    unsigned long index;
    _BitScanForward64(&index, b);
    return index;
#else  // GCC, Clang, etc.
    return __builtin_ctzll(b);
#endif
}


struct Move{
    uint8_t from;
    uint8_t to;
    uint8_t flags;
    bool inCheck{false};
};


//Change Class name 
class MoveGen{

private:
    ChessBoard& board;
    std::array<uint64_t, 64> king_attacks;
    std::array<uint64_t, 64> knight_attacks;
    std::vector<Move> moves;
    std::vector<Move> attack_vision;

    uint64_t getRankMask(int square) {
    /*
    Rank 1: 0xFF               // 0-7
    Rank 2: 0xFF00            // 8-15    (0xFF << 8)
    Rank 3: 0xFF0000          // 16-23   (0xFF << 16)
    Rank 4: 0xFF000000        // 24-31   (0xFF << 24)
    Rank 5: 0xFF00000000      // 32-39   (0xFF << 32)
    Rank 6: 0xFF0000000000    // 40-47   (0xFF << 40)
    Rank 7: 0xFF000000000000  // 48-55   (0xFF << 48)
    Rank 8: 0xFF00000000000000// 56-63   (0xFF << 56)
    */ 
        return 0xFFULL << (square & 56);  // square & 56 is same as (square / 8) * 8
    }
    

    uint64_t getFileMask(int square) {
        return 0x0101010101010101ULL << (square & 7);
    }


    uint64_t getAllPieces(){
        return board[WP] | board[WN] | board[WB] | board[WR] | board[WQ] | board[WK] | 
               board[BP] | board[BN] | board[BB] | board[BR] | board[BQ] | board[BK]; 
    }

    uint64_t getWhitePieces(){
        return board[WP] | board[WN] | board[WB] | board[WR] | board[WQ] | board[WK];
    }

    uint64_t getBlackPieces(){
        return board[BP] | board[BN] | board[BB] | board[BR] | board[BQ] | board[BK];
    }

    void GeneratePawnMoves(bool isWhite){

        uint64_t pawns = isWhite ? board[WP] : board[BP];
        uint64_t enemies = isWhite ? getBlackPieces() : getWhitePieces();
        uint64_t empty = ~getAllPieces();


        // Bits dont work with negatives 
        int direction = isWhite ? 8 : -8;
        
        uint64_t SinglePush = isWhite ? 
            (pawns << 8) & empty :    // White moves up
            (pawns >> 8) & empty;         

        // -.- make sure to EXPLICITY call it 64 bits 
        uint64_t doublePush = isWhite ? 
            ((SinglePush & 0xFF0000ULL) << 8) & empty :            // White pawns on rank 3
            ((SinglePush & 0xFF0000000000ULL) >> 8) & empty;   // Isolate rank 6

        //~0x0101010101010101ULL Makes sure pawn cant capture on the A File  
        //~0x8080808080808080ULL Make sure Pawn Cant Capture on the H File 
        uint64_t LeftCapture = isWhite ? 
            ((pawns & ~0x0101010101010101ULL) << 7) & enemies :
            ((pawns & ~0x0101010101010101ULL) >> 9) & enemies;

        uint64_t RightCaptures = isWhite ?
            ((pawns & ~0x8080808080808080ULL) << 9) & enemies :
            ((pawns & ~0x8080808080808080ULL) >> 7) & enemies;




        // Essentially jump to next ON BIT, instead of iterating through all of them discuss at Meeting (Isac)
        // __builtin_ctzll Essentailly Finds the Right most Bit
        // Save the Push 
        // SingePush - 1 removes the move and then looks for the next move again 
        while(SinglePush) {
            int to = getLSB(SinglePush);
            int from = to - direction;
            moves.push_back({(uint8_t)from, (uint8_t)to, 0});
            SinglePush &= SinglePush - 1;  // Clear least significant bit
        }
        
        while(doublePush) {
            int to = getLSB(doublePush);
            int from = to - (2 * direction);  // Subtract 2 ranks worth of movement
            moves.push_back({(uint8_t)from, (uint8_t)to, 0});
            doublePush &= doublePush - 1; 
        }

        while(LeftCapture){
            int to = getLSB(LeftCapture);
            int from = isWhite ? to - 7 : to + 9;  
            moves.push_back({(uint8_t)from, (uint8_t)to, 0});
            LeftCapture &= LeftCapture - 1; 
        }

        while(RightCaptures){
            int to = getLSB(RightCaptures);
            int from = isWhite ? to - 9 : to + 7;  // Correct diagonal math
            moves.push_back({(uint8_t)from, (uint8_t)to, 0});
            RightCaptures &= RightCaptures - 1; 
        }
    }

    void GeneratePawnAttackVision(bool isWhite) {
        uint64_t pawns = isWhite ? board[WP] : board[BP];

        uint64_t LeftCapture = isWhite ?
            ((pawns & ~0x0101010101010101ULL) << 7):
            ((pawns & ~0x0101010101010101ULL) >> 9);

        uint64_t RightCaptures = isWhite ?
            ((pawns & ~0x8080808080808080ULL) << 9):
            ((pawns & ~0x8080808080808080ULL) >> 7);

        while (LeftCapture) {
            int to = getLSB(LeftCapture);
            int from = isWhite ? to - 7 : to + 9;
            attack_vision.push_back({ (uint8_t)from, (uint8_t)to, 0 });
            LeftCapture &= LeftCapture - 1;
        }

        while (RightCaptures) {
            int to = getLSB(RightCaptures);
            int from = isWhite ? to - 9 : to + 7;  // Correct diagonal math
            attack_vision.push_back({ (uint8_t)from, (uint8_t)to, 0 });
            RightCaptures &= RightCaptures - 1;
        }
    }

    void GenerateKnightMoves(bool isWhite, std::vector<Move>& move, bool includeFriendly = false){
        uint64_t knight = isWhite ? board[WN] : board[BN];
        uint64_t friendly = isWhite ? getWhitePieces() : getBlackPieces();
        // uint64_t empty = ~getAllPieces();
        while(knight) {
            int from = getLSB(knight);
            uint64_t move_mask = 0ULL;
                if (from > 15)  { // Up 2
                if (from % 8 != 0)  move_mask |= 1ULL << (from - 17);  // Left 1
                if (from % 8 != 7)  move_mask |= 1ULL << (from - 15);  // Right 1
            }
            if (from < 48)  { // Down 2
                if (from % 8 != 0)  move_mask |= 1ULL << (from + 15);  // Left 1
                if (from % 8 != 7)  move_mask |= 1ULL << (from + 17);  // Right 1
            }
            if (from % 8 > 1)  { // Left 2
                if (from > 7)   move_mask |= 1ULL << (from - 10);  // Up 1
                if (from < 56)  move_mask |= 1ULL << (from + 6);   // Down 1
            }
            if (from % 8 < 6)  { // Right 2
                if (from > 7)   move_mask |= 1ULL << (from - 6);   // Up 1
                if (from < 56)  move_mask |= 1ULL << (from + 10);  // Down 1
            }
            
            // Remove moves to squares occupied by friendly pieces
            if (!includeFriendly) move_mask &= ~friendly;
            
            // Add all valid moves to the moves vector
            while (move_mask) {
                int to = getLSB(move_mask);
                move.push_back({(uint8_t)from, (uint8_t)to, 0});
                move_mask &= move_mask - 1;  // Clear least significant bit
            }
            
            knight &= knight - 1;  // Clear least significant bit
        }

    }
    
    void GenerateRookMoves(bool isWhite, std::vector<Move>& move, bool includeFriendly = false){
        uint64_t Rook = isWhite ? board[WR] : board[BR];
        uint64_t friendly = isWhite ? getWhitePieces() : getBlackPieces();
        uint64_t allPieces = getAllPieces();
            while(Rook){
                int from = getLSB(Rook);
                uint64_t move_mask = 0ULL;

                uint64_t rank_mask = getRankMask(from);
                uint64_t file_mask = getFileMask(from);

                uint64_t blockers = allPieces & rank_mask;
                


                    //Looking right
                uint64_t possible = rank_mask & (-(1ULL << (from + 1)));
                blockers = allPieces & rank_mask & possible;
                if (blockers) {
                    int blocker = getLSB(blockers);
                    possible &= ~(rank_mask & (-(1ULL << blocker)));
                }
                move_mask |= possible;



                // Lookign left 
                possible = rank_mask & ((1ULL << from) - 1);
                blockers = allPieces & rank_mask;
                if (blockers) {
                    int blocker = 63 - getLSB(blockers & possible);
                    possible &= ~(rank_mask & ((1ULL << (blocker + 1)) - 1));
                }
                move_mask |= possible;


                blockers = allPieces & file_mask;

                //Look up 
                possible = file_mask & (-(1ULL << (from + 1)));
                if(blockers){
                    int blocker = getLSB(blockers & possible);
                    possible &= ~(file_mask & (-(1ULL) << blocker));
                }
                move_mask |= possible;
                

             // Look Down (towards decreasing squares along the file)
                possible = file_mask & ((1ULL << from) - 1);
                blockers = allPieces & file_mask & possible;
                if (blockers) {
                    int blocker = 63 - getLSB(blockers);
                    possible &= ~(file_mask & ((1ULL << (blocker + 1)) - 1));
                }
                move_mask |= possible;

                if (!includeFriendly) move_mask &= ~friendly;

                while (move_mask) {
                    int to = getLSB(move_mask);
                    move.push_back({(uint8_t)from, (uint8_t)to, 0});
                    move_mask &= move_mask - 1;
                }
        
            Rook &= Rook - 1;
            }
    }

 void GenerateBishopMoves(bool isWhite, std::vector<Move>& move, bool includeFriendly = false) {
    uint64_t Bishop = isWhite ? board[WB] : board[BB];
    uint64_t friendly = isWhite ? getWhitePieces() : getBlackPieces();
    uint64_t allPieces = getAllPieces();
    
    while (Bishop) {
        int from = getLSB(Bishop);
        uint64_t move_mask = 0ULL;
        
        // Helper lambda to get diagonal mask
        auto getDiagonalMask = [](int square) {
            const uint64_t mainDiagonal = 0x8040201008040201ULL;
            int diag = (square % 8) - (square / 8);
            if (diag >= 0) {
                return mainDiagonal >> (diag * 8);
            } else {
                return mainDiagonal << (-diag * 8);
            }
        };
        
        // Helper lambda to get anti-diagonal mask
        auto getAntiDiagonalMask = [](int square) {
            const uint64_t mainAntiDiagonal = 0x0102040810204080ULL;
            int diag = (square % 8) + (square / 8);
            if (diag <= 7) {
                return mainAntiDiagonal >> ((7 - diag) * 8);
            } else {
                return mainAntiDiagonal << ((diag - 7) * 8);
            }
        };

        uint64_t diagonalMask = getDiagonalMask(from);
        uint64_t antiDiagonalMask = getAntiDiagonalMask(from);

        // Northeast direction (diagonal)
        uint64_t possible = diagonalMask & (-(1ULL << (from + 1)));
        uint64_t blockers = allPieces & diagonalMask & possible;
        if (blockers) {
            int blocker = getLSB(blockers);
            possible &= ~(-(1ULL << blocker));
        }
        move_mask |= possible;

        // Southwest direction (diagonal)
        possible = diagonalMask & ((1ULL << from) - 1);
        blockers = allPieces & diagonalMask & possible;
        if (blockers) {
            int blocker = 63 - getLSB(blockers);
            possible &= ~((1ULL << (blocker + 1)) - 1);
        }
        move_mask |= possible;

        // Northwest direction (anti-diagonal)
        possible = antiDiagonalMask & (-(1ULL << (from + 1)));
        blockers = allPieces & antiDiagonalMask & possible;
        if (blockers) {
            int blocker = getLSB(blockers);
            possible &= ~(-(1ULL << blocker));
        }
        move_mask |= possible;

        // Southeast direction (anti-diagonal)
        possible = antiDiagonalMask & ((1ULL << from) - 1);
        blockers = allPieces & antiDiagonalMask & possible;
        if (blockers) {
            int blocker = 63 - getLSB(blockers);
            possible &= ~((1ULL << (blocker + 1)) - 1);
        }
        move_mask |= possible;

        // Remove moves to squares occupied by friendly pieces
        if (!includeFriendly) move_mask &= ~friendly;

        // Add all valid moves to the moves vector
        while (move_mask) {
            int to = getLSB(move_mask);
            move.push_back({(uint8_t)from, (uint8_t)to, 0});
            move_mask &= move_mask - 1;
        }

        Bishop &= Bishop - 1;  // Move to next bishop
    }
}


void GenerateQueenMoves(bool isWhite, std::vector<Move>& move, bool includeFriendly = false) {
    uint64_t Queen = isWhite ? board[WQ] : board[BQ];
    uint64_t friendly = isWhite ? getWhitePieces() : getBlackPieces();
    uint64_t allPieces = getAllPieces();
    
    while (Queen) {
        int from = getLSB(Queen);
        uint64_t move_mask = 0ULL;

        // Get masks for all directions
        uint64_t rank_mask = getRankMask(from);
        uint64_t file_mask = getFileMask(from);
        
        // Reuse the diagonal mask lambdas from bishop moves
        auto getDiagonalMask = [](int square) {
            const uint64_t mainDiagonal = 0x8040201008040201ULL;
            int diag = (square % 8) - (square / 8);
            if (diag >= 0) {
                return mainDiagonal >> (diag * 8);
            } else {
                return mainDiagonal << (-diag * 8);
            }
        };
        
        auto getAntiDiagonalMask = [](int square) {
            const uint64_t mainAntiDiagonal = 0x0102040810204080ULL;
            int diag = (square % 8) + (square / 8);
            if (diag <= 7) {
                return mainAntiDiagonal >> ((7 - diag) * 8);
            } else {
                return mainAntiDiagonal << ((diag - 7) * 8);
            }
        };

        uint64_t diagonal_mask = getDiagonalMask(from);
        uint64_t anti_diagonal_mask = getAntiDiagonalMask(from);

        // Rook-like moves (horizontal and vertical)
        // Looking right
        uint64_t possible = rank_mask & (-(1ULL << (from + 1)));
        uint64_t blockers = allPieces & rank_mask & possible;
        if (blockers) {
            int blocker = getLSB(blockers);
            possible &= ~(rank_mask & (-(1ULL << blocker)));
        }
        move_mask |= possible;

        // Looking left
        possible = rank_mask & ((1ULL << from) - 1);
        blockers = allPieces & rank_mask & possible;
        if (blockers) {
            int blocker = 63 - getLSB(blockers);
            possible &= ~(rank_mask & ((1ULL << (blocker + 1)) - 1));
        }
        move_mask |= possible;

        // Looking up
        possible = file_mask & (-(1ULL << (from + 1)));
        blockers = allPieces & file_mask & possible;
        if (blockers) {
            int blocker = getLSB(blockers & possible);
            possible &= ~(file_mask & (-(1ULL << blocker)));
        }
        move_mask |= possible;

        // Looking down
        possible = file_mask & ((1ULL << from) - 1);
        blockers = allPieces & file_mask & possible;
        if (blockers) {
            int blocker = 63 - getLSB(blockers);
            possible &= ~(file_mask & ((1ULL << (blocker + 1)) - 1));
        }
        move_mask |= possible;

        // Bishop-like moves (diagonals)
        // Northeast direction
        possible = diagonal_mask & (-(1ULL << (from + 1)));
        blockers = allPieces & diagonal_mask & possible;
        if (blockers) {
            int blocker = getLSB(blockers);
            possible &= ~(-(1ULL << blocker));
        }
        move_mask |= possible;

        // Southwest direction
        possible = diagonal_mask & ((1ULL << from) - 1);
        blockers = allPieces & diagonal_mask & possible;
        if (blockers) {
            int blocker = 63 - getLSB(blockers);
            possible &= ~((1ULL << (blocker + 1)) - 1);
        }
        move_mask |= possible;

        // Northwest direction
        possible = anti_diagonal_mask & (-(1ULL << (from + 1)));
        blockers = allPieces & anti_diagonal_mask & possible;
        if (blockers) {
            int blocker = getLSB(blockers);
            possible &= ~(-(1ULL << blocker));
        }
        move_mask |= possible;

        // Southeast direction
        possible = anti_diagonal_mask & ((1ULL << from) - 1);
        blockers = allPieces & anti_diagonal_mask & possible;
        if (blockers) {
            int blocker = 63 - getLSB(blockers);
            possible &= ~((1ULL << (blocker + 1)) - 1);
        }
        move_mask |= possible;

        // Remove moves to squares occupied by friendly pieces
        if (!includeFriendly) move_mask &= ~friendly;

        // Add all valid moves to the moves vector
        while (move_mask) {
            int to = getLSB(move_mask);
            move.push_back({(uint8_t)from, (uint8_t)to, 0});
            move_mask &= move_mask - 1;
        }

        Queen &= Queen - 1;  // Move to next queen
    }
}

    void GenerateKingMoves(bool isWhite, std::vector<Move>& move, bool includeFriendly = false) {
    uint64_t King = isWhite ? board[WK] : board[BK];
    uint64_t friendly = isWhite ? getWhitePieces() : getBlackPieces();
    
    while (King) {
        int from = getLSB(King);
        uint64_t move_mask = 0ULL;
        
        // Generate all possible moves
        // Right
        if (from % 8 != 7) {
            move_mask |= 1ULL << (from + 1);
            // Up-Right
            if (from < 56) move_mask |= 1ULL << (from + 9);
            // Down-Right
            if (from > 7) move_mask |= 1ULL << (from - 7);
        }
        
        // Left
        if (from % 8 != 0) {
            move_mask |= 1ULL << (from - 1);
            // Up-Left
            if (from < 56) move_mask |= 1ULL << (from + 7);
            // Down-Left
            if (from > 7) move_mask |= 1ULL << (from - 9);
        }
        
        // Up (directly)
        if (from < 56) move_mask |= 1ULL << (from + 8);
        
        // Down (directly)
        if (from > 7) move_mask |= 1ULL << (from - 8);
        
        // Remove moves to squares occupied by friendly pieces
        if (!includeFriendly) move_mask &= ~friendly;
        
        // Add all valid moves to the moves vector
        while (move_mask) {
            int to = getLSB(move_mask);
            move.push_back({(uint8_t)from, (uint8_t)to, 0});
            move_mask &= move_mask - 1;  // Clear least significant bit
        }
        
        King &= King - 1;  // Clear the current king bit (though there's only one king)
    }
}
    public:
    MoveGen(ChessBoard& boards) : board(boards){}

    std::vector<Move> GenerateMoves(bool isWhite) {
        moves.clear();
        GeneratePawnMoves(isWhite);
        GenerateKnightMoves(isWhite, moves);
        GenerateRookMoves(isWhite, moves);
        GenerateBishopMoves(isWhite, moves);
        GenerateQueenMoves(isWhite, moves);
        GenerateKingMoves(isWhite, moves);
        return moves;
    }

    std::vector<Move> GenerateAttackVision(bool isWhite) {
        attack_vision.clear();
        GeneratePawnAttackVision(isWhite);
        GenerateKnightMoves(isWhite, attack_vision, true);
        GenerateRookMoves(isWhite, attack_vision, true);
        GenerateBishopMoves(isWhite, attack_vision, true);
        GenerateQueenMoves(isWhite, attack_vision, true);
        GenerateKingMoves(isWhite, attack_vision, true);
        return attack_vision;
    }
};