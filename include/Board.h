#pragma once
#include "Piece.h"
#include <array>
#include <string>


using namespace std;


// ============================================================
//  Board.h  –  Chess board class
//  OOP Concepts: Encapsulation, Composition
// ============================================================

struct Move {
    Position from, to;
    bool isEnPassant   = false;
    bool isCastling    = false;
    bool isPromotion   = false;
    PieceType promoteTo = PieceType::QUEEN;

    Move() = default;
    Move(Position f, Position t) : from(f), to(t) {}
};

class Board {
public:
    // 8×8 grid of piece pointers (nullptr = empty)
    array<array<shared_ptr<Piece>, 8>, 8> grid;

    // En-passant target square (-1 = none)
    Position enPassantTarget;

    Color currentTurn;

    // Castling rights
    bool whiteKingsideCastle  = true;
    bool whiteQueensideCastle = true;
    bool blackKingsideCastle  = true;
    bool blackQueensideCastle = true;

    Board();
    void initialize();                      // Set up starting position
    void display()       const;             // Print to stdout
    void displayFancy()  const;             // Color-coded display

    shared_ptr<Piece> getPiece(Position p) const;
    shared_ptr<Piece> getPiece(int row, int col) const;
    void setPiece(Position p, shared_ptr<Piece> piece);
    void clearSquare(Position p);

    bool isOccupied(Position p) const;
    bool isOccupiedByColor(Position p, Color c) const;
    bool isEmpty(Position p) const;

    // Move execution
    void applyMove(const Move& mv);
    bool isInCheck(Color color) const;
    bool isSquareAttacked(Position p, Color byColor) const;

    // Deep copy of board state (needed for AI look-ahead)
    Board clone() const;

    // Utility
    Position findKing(Color color) const;
    vector<Move> getAllLegalMoves(Color color) const;
    bool isCheckmate(Color color) const;
    bool isStalemate(Color color) const;

private:
    // Helpers for applying special moves
    void applyCastling(const Move& mv);
    void applyEnPassant(const Move& mv);
    void applyPromotion(const Move& mv);
};
