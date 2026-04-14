#include "../include/AI.h"
#include "../include/Pieces.h"
#include <algorithm>
#include <climits>


using namespace std;


// ============================================================
//  AI.cpp  –  Minimax + Alpha-Beta Pruning + Piece-Square Tables
//  OOP Concepts: Encapsulation, Single Responsibility
// ============================================================

// ────────────────────────────────────────────────────────────
//  Piece-Square Tables (positional bonuses, from White's POV)
//  Source: Simplified Evaluation Function (Chess Programming Wiki)
// ────────────────────────────────────────────────────────────
namespace PST {

const int PAWN_TABLE[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0 },
    { 50, 50, 50, 50, 50, 50, 50, 50 },
    { 10, 10, 20, 30, 30, 20, 10, 10 },
    {  5,  5, 10, 25, 25, 10,  5,  5 },
    {  0,  0,  0, 20, 20,  0,  0,  0 },
    {  5, -5,-10,  0,  0,-10, -5,  5 },
    {  5, 10, 10,-20,-20, 10, 10,  5 },
    {  0,  0,  0,  0,  0,  0,  0,  0 }
};

const int KNIGHT_TABLE[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50 },
    {-40,-20,  0,  0,  0,  0,-20,-40 },
    {-30,  0, 10, 15, 15, 10,  0,-30 },
    {-30,  5, 15, 20, 20, 15,  5,-30 },
    {-30,  0, 15, 20, 20, 15,  0,-30 },
    {-30,  5, 10, 15, 15, 10,  5,-30 },
    {-40,-20,  0,  5,  5,  0,-20,-40 },
    {-50,-40,-30,-30,-30,-30,-40,-50 }
};

const int BISHOP_TABLE[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20 },
    {-10,  0,  0,  0,  0,  0,  0,-10 },
    {-10,  0,  5, 10, 10,  5,  0,-10 },
    {-10,  5,  5, 10, 10,  5,  5,-10 },
    {-10,  0, 10, 10, 10, 10,  0,-10 },
    {-10, 10, 10, 10, 10, 10, 10,-10 },
    {-10,  5,  0,  0,  0,  0,  5,-10 },
    {-20,-10,-10,-10,-10,-10,-10,-20 }
};

const int ROOK_TABLE[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0 },
    {  5, 10, 10, 10, 10, 10, 10,  5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    {  0,  0,  0,  5,  5,  0,  0,  0 }
};

const int QUEEN_TABLE[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20 },
    {-10,  0,  0,  0,  0,  0,  0,-10 },
    {-10,  0,  5,  5,  5,  5,  0,-10 },
    { -5,  0,  5,  5,  5,  5,  0, -5 },
    {  0,  0,  5,  5,  5,  5,  0, -5 },
    {-10,  5,  5,  5,  5,  5,  0,-10 },
    {-10,  0,  5,  0,  0,  0,  0,-10 },
    {-20,-10,-10, -5, -5,-10,-10,-20 }
};

const int KING_MG_TABLE[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-20,-30,-30,-40,-40,-30,-30,-20 },
    {-10,-20,-20,-20,-20,-20,-20,-10 },
    { 20, 20,  0,  0,  0,  0, 20, 20 },
    { 20, 30, 10,  0,  0, 10, 30, 20 }
};

const int KING_EG_TABLE[8][8] = {
    {-50,-40,-30,-20,-20,-30,-40,-50 },
    {-30,-20,-10,  0,  0,-10,-20,-30 },
    {-30,-10, 20, 30, 30, 20,-10,-30 },
    {-30,-10, 30, 40, 40, 30,-10,-30 },
    {-30,-10, 30, 40, 40, 30,-10,-30 },
    {-30,-10, 20, 30, 30, 20,-10,-30 },
    {-30,-30,  0,  0,  0,  0,-30,-30 },
    {-50,-30,-30,-30,-30,-30,-30,-50 }
};

} // namespace PST

// ────────────────────────────────────────────────────────────
//  Piece-Square Table lookup
// ────────────────────────────────────────────────────────────
int ChessAI::getPSTValue(const Piece& piece) const {
    int r = piece.getPosition().row;
    int c = piece.getPosition().col;

    // White reads table top-to-bottom = rank8→rank1, so mirror row for it
    if (piece.isWhite()) r = 7 - r;

    switch (piece.getType()) {
        case PieceType::PAWN:   return PST::PAWN_TABLE  [r][c];
        case PieceType::KNIGHT: return PST::KNIGHT_TABLE[r][c];
        case PieceType::BISHOP: return PST::BISHOP_TABLE[r][c];
        case PieceType::ROOK:   return PST::ROOK_TABLE  [r][c];
        case PieceType::QUEEN:  return PST::QUEEN_TABLE [r][c];
        case PieceType::KING:   return PST::KING_MG_TABLE[r][c]; // simplified: always MG
        default: return 0;
    }
}

// ────────────────────────────────────────────────────────────
//  Static evaluation (positive = good for WHITE)
// ────────────────────────────────────────────────────────────
int ChessAI::evaluate(const Board& board) const {
    int score = 0;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            auto& pc = board.grid[r][c];
            if (!pc) continue;
            int val = pc->getValue() + getPSTValue(*pc);
            score += pc->isWhite() ? val : -val;
        }
    return score;
}

// ────────────────────────────────────────────────────────────
//  MVV-LVA score for move ordering (captures first)
// ────────────────────────────────────────────────────────────
int ChessAI::mvvLva(const Move& mv, const Board& board) const {
    auto victim   = board.getPiece(mv.to);
    auto attacker = board.getPiece(mv.from);
    if (!victim || !attacker) return 0;
    return victim->getValue() - attacker->getValue() / 10;
}

vector<Move> ChessAI::orderMoves(const vector<Move>& moves,
                                       const Board& board) const {
    vector<pair<int,Move>> scored;
    scored.reserve(moves.size());
    for (auto& mv : moves) {
        int score = 0;
        if (board.isOccupied(mv.to))
            score += 10000 + mvvLva(mv, board); // captures first
        if (mv.isPromotion)
            score += 9000;
        scored.push_back({score, mv});
    }
    sort(scored.begin(), scored.end(),
              [](auto& a, auto& b){ return a.first > b.first; });

    vector<Move> ordered;
    ordered.reserve(moves.size());
    for (auto& item : scored) ordered.push_back(item.second);
    return ordered;
}

// ────────────────────────────────────────────────────────────
//  Minimax with Alpha-Beta Pruning
//  depth    – remaining plies to search
//  alpha    – best already secured for maximiser
//  beta     – best already secured for minimiser
//  isMaximising – true when it's the AI (White) maximising
// ────────────────────────────────────────────────────────────
int ChessAI::minimax(Board& board, int depth, int alpha, int beta,
                     bool isMaximising, Color aiColor) {
    ++nodesExplored_;

    Color toMove = isMaximising ? aiColor
                                : (aiColor == Color::WHITE ? Color::BLACK : Color::WHITE);

    // Terminal / horizon
    if (depth == 0) return evaluate(board);

    auto moves = board.getAllLegalMoves(toMove);

    if (moves.empty()) {
        if (board.isInCheck(toMove))
            return isMaximising ? -100000 - depth : 100000 + depth; // Checkmate
        return 0; // Stalemate
    }

    moves = orderMoves(moves, board);

    if (isMaximising) {
        int maxEval = INT_MIN;
        for (auto& mv : moves) {
            Board copy = board.clone();
            copy.applyMove(mv);
            int eval = minimax(copy, depth-1, alpha, beta, false, aiColor);
            maxEval  = max(maxEval, eval);
            alpha    = max(alpha, eval);
            if (beta <= alpha) break; // ✂ Beta cut-off
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (auto& mv : moves) {
            Board copy = board.clone();
            copy.applyMove(mv);
            int eval = minimax(copy, depth-1, alpha, beta, true, aiColor);
            minEval  = min(minEval, eval);
            beta     = min(beta, eval);
            if (beta <= alpha) break; // ✂ Alpha cut-off
        }
        return minEval;
    }
}

// ────────────────────────────────────────────────────────────
//  Root call – returns the best move
// ────────────────────────────────────────────────────────────
Move ChessAI::getBestMove(Board& board) {
    nodesExplored_ = 0;
    Color aiColor  = board.currentTurn;
    bool maximise  = true; // AI always tries to maximise its own score

    auto moves = board.getAllLegalMoves(aiColor);
    if (moves.empty()) return Move(); // no moves (shouldn't happen in a live game)

    moves = orderMoves(moves, board);

    Move  bestMove  = moves[0];
    int   bestScore = INT_MIN;
    int   alpha     = INT_MIN;
    int   beta      = INT_MAX;

    for (auto& mv : moves) {
        Board copy  = board.clone();
        copy.applyMove(mv);
        int score   = minimax(copy, searchDepth_-1, alpha, beta, false, aiColor);
        if (score > bestScore) {
            bestScore = score;
            bestMove  = mv;
        }
        alpha = max(alpha, bestScore);
    }
    return bestMove;
}
