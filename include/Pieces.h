#pragma once
#include "Piece.h"
#include "Board.h"


using namespace std;


// ============================================================
//  Pieces.h  –  Concrete piece classes (Inheritance + Polymorphism)
// ============================================================

// ──────────────────────────────────────────────────────────────
//  Pawn
// ──────────────────────────────────────────────────────────────
class Pawn : public Piece {
public:
    Pawn(Color color, Position pos) : Piece(color, PieceType::PAWN, pos) {}
    vector<Position> getLegalMoves(const Board& board) const override;
    int  getValue()  const override { return 100; }
    char getSymbol() const override { return isWhite() ? 'P' : 'p'; }
    unique_ptr<Piece> clone() const override {
        auto p = make_unique<Pawn>(color_, pos_);
        p->setHasMoved(hasMoved_);
        return p;
    }
};

// ──────────────────────────────────────────────────────────────
//  Knight
// ──────────────────────────────────────────────────────────────
class Knight : public Piece {
public:
    Knight(Color color, Position pos) : Piece(color, PieceType::KNIGHT, pos) {}
    vector<Position> getLegalMoves(const Board& board) const override;
    int  getValue()  const override { return 320; }
    char getSymbol() const override { return isWhite() ? 'N' : 'n'; }
    unique_ptr<Piece> clone() const override {
        auto p = make_unique<Knight>(color_, pos_);
        p->setHasMoved(hasMoved_);
        return p;
    }
};

// ──────────────────────────────────────────────────────────────
//  Bishop
// ──────────────────────────────────────────────────────────────
class Bishop : public Piece {
public:
    Bishop(Color color, Position pos) : Piece(color, PieceType::BISHOP, pos) {}
    vector<Position> getLegalMoves(const Board& board) const override;
    int  getValue()  const override { return 330; }
    char getSymbol() const override { return isWhite() ? 'B' : 'b'; }
    unique_ptr<Piece> clone() const override {
        auto p = make_unique<Bishop>(color_, pos_);
        p->setHasMoved(hasMoved_);
        return p;
    }
};

// ──────────────────────────────────────────────────────────────
//  Rook
// ──────────────────────────────────────────────────────────────
class Rook : public Piece {
public:
    Rook(Color color, Position pos) : Piece(color, PieceType::ROOK, pos) {}
    vector<Position> getLegalMoves(const Board& board) const override;
    int  getValue()  const override { return 500; }
    char getSymbol() const override { return isWhite() ? 'R' : 'r'; }
    unique_ptr<Piece> clone() const override {
        auto p = make_unique<Rook>(color_, pos_);
        p->setHasMoved(hasMoved_);
        return p;
    }
};

// ──────────────────────────────────────────────────────────────
//  Queen
// ──────────────────────────────────────────────────────────────
class Queen : public Piece {
public:
    Queen(Color color, Position pos) : Piece(color, PieceType::QUEEN, pos) {}
    vector<Position> getLegalMoves(const Board& board) const override;
    int  getValue()  const override { return 900; }
    char getSymbol() const override { return isWhite() ? 'Q' : 'q'; }
    unique_ptr<Piece> clone() const override {
        auto p = make_unique<Queen>(color_, pos_);
        p->setHasMoved(hasMoved_);
        return p;
    }
};

// ──────────────────────────────────────────────────────────────
//  King
// ──────────────────────────────────────────────────────────────
class King : public Piece {
public:
    King(Color color, Position pos) : Piece(color, PieceType::KING, pos) {}
    vector<Position> getLegalMoves(const Board& board) const override;
    int  getValue()  const override { return 20000; }
    char getSymbol() const override { return isWhite() ? 'K' : 'k'; }
    unique_ptr<Piece> clone() const override {
        auto p = make_unique<King>(color_, pos_);
        p->setHasMoved(hasMoved_);
        return p;
    }
};
