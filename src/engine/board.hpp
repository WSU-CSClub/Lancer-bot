#include <iostream>
#include <vector>
#include <cstdint>

// Bitboard for a single piece type
using Bitboard = uint64_t;

// Enum for piece types
enum Piece { WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK, EMPTY };

void setPiece(Bitboard &bitboard, int file, int rank) {
    int square = rank * 8 + file;
    bitboard |= (1ULL << square);
    std::cout << "Setting piece at file " << file << ", rank " << rank 
              << " (square " << square << ")" << std::endl;
}

void printBoard(const std::vector<Bitboard>& pieceBitboards) {
    char board[64];
    std::fill(std::begin(board), std::end(board), '.'); // Fill board with empty squares

    // Map each square to the appropriate piece character
    for (int i = 0; i < pieceBitboards.size(); ++i) {
        Bitboard bitboard = pieceBitboards[i];
        for (int square = 0; square < 64; ++square) {
            if ((bitboard >> square) & 1ULL) {
                switch (i) {
                    case WP: board[square] = 'P'; break;
                    case WN: board[square] = 'N'; break;
                    case WB: board[square] = 'B'; break;
                    case WR: board[square] = 'R'; break;
                    case WQ: board[square] = 'Q'; break;
                    case WK: board[square] = 'K'; break;
                    case BP: board[square] = 'p'; break;
                    case BN: board[square] = 'n'; break;
                    case BB: board[square] = 'b'; break;
                    case BR: board[square] = 'r'; break;
                    case BQ: board[square] = 'q'; break;
                    case BK: board[square] = 'k'; break;
                }
            }
        }
    }

    // Print the board 
    std::cout << "\nChess Board:\n";
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << rank + 1 << " ";  // Rank label
        for (int file = 0; file < 8; ++file) {
            std::cout << board[rank * 8 + file] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "  a b c d e f g h\n";  // File labels
}

int main() {
    // Example bitboard
    std::vector<Bitboard> pieceBitboards(12, 0);
    setPiece(pieceBitboards[WP], 0, 1);  // a2 (file 0, rank 1)
    setPiece(pieceBitboards[BP], 0, 3);  // a4 (file 0, rank 3)
    setPiece(pieceBitboards[WN], 1, 0);  // b1 (file 1, rank 0)
    setPiece(pieceBitboards[BQ], 5, 6);

    // Print the board
    printBoard(pieceBitboards);

    return 0;
}
