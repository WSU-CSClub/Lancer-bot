#include "board.hpp"
#include <iostream>
#include <vector>

void setPiece(Bitboard &bitboard, int file, int rank) {
  int square = rank * 8 + file;
  bitboard |= (1ULL << square);
  std::cout << "Setting piece at file " << file << ", rank " << rank
            << " (square " << square << ")" << std::endl;
}

void initBoard(ChessBoard &b) {

  // Set up pieces for White
  setPiece(b[WR], 0, 0); // White Rook on a1
  setPiece(b[WN], 1, 0); // White Knight on b1
  setPiece(b[WB], 2, 0); // White Bishop on c1
  setPiece(b[WQ], 3, 0); // White Queen on d1
  setPiece(b[WK], 4, 0); // White King on e1
  setPiece(b[WB], 5, 0); // White Bishop on f1
  setPiece(b[WN], 6, 0); // White Knight on g1
  setPiece(b[WR], 7, 0); // White Rook on h1
  for (int file = 0; file < 8; ++file) {
    setPiece(b[WP], file, 1); // White Pawns on rank 2
  }

  // Set up pieces for Black
  setPiece(b[BR], 0, 7); // Black Rook on a8
  setPiece(b[BN], 1, 7); // Black Knight on b8
  setPiece(b[BB], 2, 7); // Black Bishop on c8
  setPiece(b[BQ], 3, 7); // Black Queen on d8
  setPiece(b[BK], 4, 7); // Black King on e8
  setPiece(b[BB], 5, 7); // Black Bishop on f8
  setPiece(b[BN], 6, 7); // Black Knight on g8
  setPiece(b[BR], 7, 7); // Black Rook on h8
  for (int file = 0; file < 8; ++file) {
    setPiece(b[BP], file, 6); // Black Pawns on rank 7
  }
}



void printBoard(const ChessBoard &pieceBitboards) {
  char board[64];

  // Fill board with empty squares
  std::fill(std::begin(board), std::end(board), '.');

  // Map each square to the appropriate piece character
  for (int i = 0; i < pieceBitboards.size(); ++i) {
    Bitboard bitboard = pieceBitboards[i];

    for (int square = 0; square < 64; ++square) {
      if (!((bitboard >> square) & 1ULL)) {
        continue;
      }

      switch (i) {
      case WP:
        board[square] = 'P';
        break;
      case WN:
        board[square] = 'N';
        break;
      case WB:
        board[square] = 'B';
        break;
      case WR:
        board[square] = 'R';
        break;
      case WQ:
        board[square] = 'Q';
        break;
      case WK:
        board[square] = 'K';
        break;
      case BP:
        board[square] = 'p';
        break;
      case BN:
        board[square] = 'n';
        break;
      case BB:
        board[square] = 'b';
        break;
      case BR:
        board[square] = 'r';
        break;
      case BQ:
        board[square] = 'q';
        break;
      case BK:
        board[square] = 'k';
        break;
      }
    }
  }

  // Print the board
  std::cout << "\nChess Board:\n";
  for (int rank = 7; rank >= 0; --rank) {
    std::cout << rank + 1 << " "; // Rank label
    for (int file = 0; file < 8; ++file) {
      std::cout << board[rank * 8 + file] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "  a b c d e f g h\n"; // File labels
}

void fenToBoard(ChessBoard &board, const std::string &fen) {
    // Clear the board first
    for (auto &bitboard : board) {
        bitboard = 0ULL;
    }
    
    int rank = 7;  // Start from rank 8 (index 7)
    int file = 0;  // Start from file a (index 0)
    
    // Parse piece positions
    size_t pos = 0;
    while (pos < fen.length() && fen[pos] != ' ') {
        char c = fen[pos];
        
        if (c == '/') {
            rank--;
            file = 0;
        }
        else if (isdigit(c)) {
            file += (c - '0');
        }
        else {
            // Map piece characters to board indices
            int pieceIndex;
            switch (c) {
                case 'P': pieceIndex = WP; break;
                case 'N': pieceIndex = WN; break;
                case 'B': pieceIndex = WB; break;
                case 'R': pieceIndex = WR; break;
                case 'Q': pieceIndex = WQ; break;
                case 'K': pieceIndex = WK; break;
                case 'p': pieceIndex = BP; break;
                case 'n': pieceIndex = BN; break;
                case 'b': pieceIndex = BB; break;
                case 'r': pieceIndex = BR; break;
                case 'q': pieceIndex = BQ; break;
                case 'k': pieceIndex = BK; break;
                default: 
                    throw std::runtime_error("Invalid FEN character: " + std::string(1, c));
            }
            
            setPiece(board[pieceIndex], file, rank);
            file++;
        }
        pos++;
    }
    
    // Note: Additional FEN fields (active color, castling rights, en passant, etc.)
    // can be parsed here if needed for future functionality
}


void setPositionFromFEN(ChessBoard &board, const std::string &fen) {
    // Clear all bitboards first
    for (auto &bitboard : board) {
        bitboard = 0ULL;
    }
    
    int rank = 7;  // Start from rank 8 (index 7)
    int file = 0;  // Start from file a (index 0)
    
    // Parse the piece placement part of FEN (before first space)
    size_t pos = fen.find(' ');
    std::string piece_placement = fen.substr(0, pos);
    
    for (char c : piece_placement) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (isdigit(c)) {
            file += (c - '0');
        } else {
            int square = rank * 8 + file;
            Bitboard mask = 1ULL << square;
            
            switch (c) {
                // White pieces
                case 'P': board[WP] |= mask; break;
                case 'N': board[WN] |= mask; break;
                case 'B': board[WB] |= mask; break;
                case 'R': board[WR] |= mask; break;
                case 'Q': board[WQ] |= mask; break;
                case 'K': board[WK] |= mask; break;
                
                // Black pieces
                case 'p': board[BP] |= mask; break;
                case 'n': board[BN] |= mask; break;
                case 'b': board[BB] |= mask; break;
                case 'r': board[BR] |= mask; break;
                case 'q': board[BQ] |= mask; break;
                case 'k': board[BK] |= mask; break;
            }
            file++;
        }
    }
}


std::string boardToFen(const ChessBoard &board) {
    std::string fen;
    char pieceSymbols[12] = {'P','N','B','R','Q','K','p','n','b','r','q','k'};
    
    for (int rank = 7; rank >= 0; rank--) {
        int emptyCount = 0;
        
        for (int file = 0; file < 8; file++) {
            bool pieceFound = false;
            
            for (int pieceType = 0; pieceType < 12; pieceType++) {
                if (board[pieceType] & (1ULL << (rank * 8 + file))) {
                    if (emptyCount > 0) {
                        fen += std::to_string(emptyCount);
                        emptyCount = 0;
                    }
                    fen += pieceSymbols[pieceType];
                    pieceFound = true;
                    break;
                }
            }
            
            if (!pieceFound) {
                emptyCount++;
            }
        }
        
        if (emptyCount > 0) {
            fen += std::to_string(emptyCount);
        }
        
        if (rank > 0) {
            fen += '/';
        }
    }
    
    // Add standard suffix for initial position
    fen += " w KQkq - 0 1";
    return fen;
}


