#pragma once
#include <vector>
#include <string>
#include <memory>


using namespace std;


// ============================================================
//  Piece.h  –  Abstract base class for all chess pieces
//  OOP Concepts: Abstraction, Encapsulation, Inheritance
// ============================================================

enum class Color { WHITE, BLACK, NONE };
enum class PieceType { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, EMPTY };

struct Position {
    int row, col;
    Position(int r = -1, int c = -1) : row(r), col(c) {}
    bool isValid() const { return row >= 0 && row < 8 && col >= 0 && col < 8; }
    bool operator==(const Position& o) const { return row == o.row && col == o.col; }
    bool operator!=(const Position& o) const { return !(*this == o); }
};

class Board; // Forward declaration

class Piece {
protected:
    Color  color_;
    PieceType type_;
    Position  pos_;
    bool      hasMoved_;

public:
    Piece(Color color, PieceType type, Position pos)
        : color_(color), type_(type), pos_(pos), hasMoved_(false) {}
    virtual ~Piece() = default;

    // Pure virtual – each subclass defines its own move logic
    virtual vector<Position> getLegalMoves(const Board& board) const = 0;
    virtual int  getValue()   const = 0;   // Material value
    virtual char getSymbol()  const = 0;   // ASCII symbol
    virtual unique_ptr<Piece> clone() const = 0;

    // Accessors
    Color     getColor()    const { return color_; }
    PieceType getType()     const { return type_;  }
    Position  getPosition() const { return pos_;   }
    bool      hasMoved()    const { return hasMoved_; }

    // Mutators
    void setPosition(Position p)  { pos_     = p;    }
    void setHasMoved(bool moved)  { hasMoved_ = moved; }

    bool isWhite() const { return color_ == Color::WHITE; }
    bool isBlack() const { return color_ == Color::BLACK; }
};
