#pragma once
#include "Board.h"
#include <limits>
#include <chrono>


using namespace std;


// ============================================================
//  AI.h  –  Minimax with Alpha-Beta Pruning
//  OOP Concepts: Encapsulation, Single Responsibility
// ============================================================

// Piece-square tables (positional bonuses)
namespace PST {
    extern const int PAWN_TABLE  [8][8];
    extern const int KNIGHT_TABLE[8][8];
    extern const int BISHOP_TABLE[8][8];
    extern const int ROOK_TABLE  [8][8];
    extern const int QUEEN_TABLE [8][8];
    extern const int KING_MG_TABLE[8][8]; // Middlegame
    extern const int KING_EG_TABLE[8][8]; // Endgame
}

class ChessAI {
public:
    explicit ChessAI(int depth = 4) : searchDepth_(depth), nodesExplored_(0) {}

    // Returns the best move for the current player
    Move getBestMove(Board& board);

    // Accessors
    int  getNodesExplored() const { return nodesExplored_; }
    void setDepth(int d)          { searchDepth_ = d; }

private:
    int searchDepth_;
    int nodesExplored_;

    // Core Minimax with Alpha-Beta
    int minimax(Board& board, int depth, int alpha, int beta,
                bool isMaximising, Color aiColor);

    // Static evaluation
    int evaluate(const Board& board) const;
    int evaluatePiece(const Piece& piece) const;
    int getPSTValue(const Piece& piece) const;

    // Move ordering (for better alpha-beta cutoffs)
    vector<Move> orderMoves(const vector<Move>& moves,
                                 const Board& board) const;
    int mvvLva(const Move& mv, const Board& board) const; // Most Valuable Victim – Least Valuable Attacker
};
