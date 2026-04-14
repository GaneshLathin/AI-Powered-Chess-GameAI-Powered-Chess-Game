    #include "../include/Pieces.h"
    #include "../include/Board.h"
    #include <algorithm>


    using namespace std;


    // ============================================================
    //  Pieces.cpp  –  Move generation for each piece type
    // ============================================================

    // ────────────────────────────────────────────────────────────
    //  Helper: slide in a direction until blocked
    // ────────────────────────────────────────────────────────────
    static void slidingMoves(const Board& board, Position pos, Color color,
                            const vector<pair<int,int>>& dirs,
                            vector<Position>& out) {
        for (const auto& dir : dirs) {
            int r = pos.row + dir.first;
            int c = pos.col + dir.second;
            while (r >= 0 && r < 8 && c >= 0 && c < 8) {
                Position p(r,c);
                if (board.isEmpty(p)) {
                    out.push_back(p);
                } else {
                    if (!board.isOccupiedByColor(p, color))
                        out.push_back(p); // capture
                    break;               // blocked
                }
                r += dir.first; c += dir.second;
            }
        }
    }

    // ────────────────────────────────────────────────────────────
    //  Pawn
    // ────────────────────────────────────────────────────────────
    vector<Position> Pawn::getLegalMoves(const Board& board) const {
        vector<Position> moves;
        int dir     = isWhite() ? -1 : 1;   // White moves up (row decreases)
        int startRow = isWhite() ?  6 : 1;

        Position fwd(pos_.row + dir, pos_.col);
        if (fwd.isValid() && board.isEmpty(fwd)) {
            moves.push_back(fwd);
            // Double push from start
            Position dbl(pos_.row + 2*dir, pos_.col);
            if (pos_.row == startRow && board.isEmpty(dbl))
                moves.push_back(dbl);
        }

        // Diagonal captures
        for (int dc : {-1, 1}) {
            Position diag(pos_.row + dir, pos_.col + dc);
            if (!diag.isValid()) continue;
            // Normal capture
            if (board.isOccupied(diag) && !board.isOccupiedByColor(diag, color_))
                moves.push_back(diag);
            // En passant
            if (diag == board.enPassantTarget)
                moves.push_back(diag);
        }
        return moves;
    }

    // ────────────────────────────────────────────────────────────
    //  Knight
    // ────────────────────────────────────────────────────────────
    vector<Position> Knight::getLegalMoves(const Board& board) const {
        vector<Position> moves;
        static const int dr[] = {-2,-2,-1,-1, 1, 1, 2, 2};
        static const int dc[] = {-1, 1,-2, 2,-2, 2,-1, 1};
        for (int i = 0; i < 8; ++i) {
            Position p(pos_.row+dr[i], pos_.col+dc[i]);
            if (p.isValid() && !board.isOccupiedByColor(p, color_))
                moves.push_back(p);
        }
        return moves;
    }

    // ────────────────────────────────────────────────────────────
    //  Bishop  (diagonal)
    // ────────────────────────────────────────────────────────────
    vector<Position> Bishop::getLegalMoves(const Board& board) const {
        vector<Position> moves;
        slidingMoves(board, pos_, color_, {{-1,-1},{-1,1},{1,-1},{1,1}}, moves);
        return moves;
    }

    // ────────────────────────────────────────────────────────────
    //  Rook  (horizontal + vertical)
    // ────────────────────────────────────────────────────────────
    vector<Position> Rook::getLegalMoves(const Board& board) const {
        vector<Position> moves;
        slidingMoves(board, pos_, color_, {{-1,0},{1,0},{0,-1},{0,1}}, moves);
        return moves;
    }

    // ────────────────────────────────────────────────────────────
    //  Queen  (rook + bishop combined)
    // ────────────────────────────────────────────────────────────
    vector<Position> Queen::getLegalMoves(const Board& board) const {
        vector<Position> moves;
        slidingMoves(board, pos_, color_,
            {{-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1}}, moves);
        return moves;
    }

    // ────────────────────────────────────────────────────────────
    //  King  (one square any direction + castling)
    // ────────────────────────────────────────────────────────────
    vector<Position> King::getLegalMoves(const Board& board) const {
        vector<Position> moves;
        static const int dr[] = {-1,-1,-1, 0, 0, 1, 1, 1};
        static const int dc[] = {-1, 0, 1,-1, 1,-1, 0, 1};
        for (int i = 0; i < 8; ++i) {
            Position p(pos_.row+dr[i], pos_.col+dc[i]);
            if (p.isValid() && !board.isOccupiedByColor(p, color_))
                moves.push_back(p);
        }

        // Castling
        if (!hasMoved_) {
            Color opp = isWhite() ? Color::BLACK : Color::WHITE;
            int row   = pos_.row;

            auto canCastle = [&](bool right, bool& castleFlag) {
                if (!castleFlag) return;
                int rookCol  = right ? 7 : 0;
                int dir      = right ? 1 : -1;
                // Check squares between king and rook are empty
                for (int c = pos_.col + dir; c != rookCol; c += dir)
                    if (!board.isEmpty(Position(row,c))) return;
                // Check king doesn't pass through check
                for (int c = pos_.col; c != pos_.col + 2*dir + dir; c += dir)
                    if (board.isSquareAttacked(Position(row,c), opp)) return;
                // Rook must be there
                auto rook = board.getPiece(Position(row, rookCol));
                if (!rook || rook->getType() != PieceType::ROOK ||
                    rook->getColor() != color_ || rook->hasMoved()) return;
                moves.push_back(Position(row, pos_.col + 2*dir));
            };

            bool kSide = isWhite() ? board.whiteKingsideCastle  : board.blackKingsideCastle;
            bool qSide = isWhite() ? board.whiteQueensideCastle : board.blackQueensideCastle;
            canCastle(true,  kSide);
            canCastle(false, qSide);
        }
        return moves;
    }
