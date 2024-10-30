#pragma once 
#include <iostream>
#include "board.hpp"

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
};


//Change Class name 
class MoveGen{

private:
    ChessBoard& board;
    std::array<uint64_t, 64> king_attacks;
    std::array<uint64_t, 64> knight_attacks;
    std::vector<Move> moves;

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
            int to = __builtin_ctzll(SinglePush);
            int from = to - direction;
            moves.push_back({(uint8_t)from, (uint8_t)to, 0});
            SinglePush &= SinglePush - 1;  // Clear least significant bit
        }
        
        while(doublePush) {
            int to = __builtin_ctzll(doublePush);
            int from = to - (2 * direction);  // Subtract 2 ranks worth of movement
            moves.push_back({(uint8_t)from, (uint8_t)to, 0});
            doublePush &= doublePush - 1; 
        }

        while(LeftCapture){
            int to = __builtin_ctzll(LeftCapture);
            int from = isWhite ? to - 7 : to + 9;  
            moves.push_back({(uint8_t)from, (uint8_t)to, 0});
            LeftCapture &= LeftCapture - 1; 
        }

        while(RightCaptures){
            int to = __builtin_ctzll(RightCaptures);
            int from = isWhite ? to - 9 : to + 7;  // Correct diagonal math
            moves.push_back({(uint8_t)from, (uint8_t)to, 0});
            RightCaptures &= RightCaptures - 1; 
        }
    }

    void generateKnightMoves(bool isWhite){
        uint64_t knight = isWhite ? board[WN] : board[BN];
        uint64_t friendly = isWhite ? getWhitePieces() : getBlackPieces();
        // uint64_t empty = ~getAllPieces();
        while(knight) {
            int from = getLSB(knight);
            uint64_t moves_mask = 0ULL;
                if (from > 15)  { // Up 2
                if (from % 8 != 0)  moves_mask |= 1ULL << (from - 17);  // Left 1
                if (from % 8 != 7)  moves_mask |= 1ULL << (from - 15);  // Right 1
            }
            if (from < 48)  { // Down 2
                if (from % 8 != 0)  moves_mask |= 1ULL << (from + 15);  // Left 1
                if (from % 8 != 7)  moves_mask |= 1ULL << (from + 17);  // Right 1
            }
            if (from % 8 > 1)  { // Left 2
                if (from > 7)   moves_mask |= 1ULL << (from - 10);  // Up 1
                if (from < 56)  moves_mask |= 1ULL << (from + 6);   // Down 1
            }
            if (from % 8 < 6)  { // Right 2
                if (from > 7)   moves_mask |= 1ULL << (from - 6);   // Up 1
                if (from < 56)  moves_mask |= 1ULL << (from + 10);  // Down 1
            }
            
            // Remove moves to squares occupied by friendly pieces
            moves_mask &= ~friendly;
            
            // Add all valid moves to the moves vector
            while (moves_mask) {
                int to = getLSB(moves_mask);
                moves.push_back({(uint8_t)from, (uint8_t)to, 0});
                moves_mask &= moves_mask - 1;  // Clear least significant bit
            }
            
            knight &= knight - 1;  // Clear least significant bit
        }

        }
    
    void GenerateRookMoves(bool isWhite){
        uint64_t Rook = isWhite ? board[WR] : board[BR];
        uint64_t friendly = isWhite ? getWhitePieces() : getBlackPieces();
        uint64_t allPieces = getAllPieces();
            while(Rook){

            }

    void GenerateBishopMoves(bool isWhite);

    }
    public:
    MoveGen(ChessBoard& boards) : board(boards){}

    std::vector<Move> GenerateMoves(bool isWhite) {
        moves.clear();
        // GeneratePawnMoves(isWhite);
        generateKnightMoves(isWhite);
        return moves;
    }
};