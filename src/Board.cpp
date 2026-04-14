#include "../include/Board.h"
#include "../include/Pieces.h"
#include <iostream>
#include <algorithm>


using namespace std;


// ============================================================
//  Board.cpp  –  Board initialisation, display, move execution
// ============================================================

Board::Board() {
    for (auto& row : grid)
        for (auto& cell : row)
            cell = nullptr;
    enPassantTarget = Position(-1, -1);
    currentTurn     = Color::WHITE;
}

void Board::initialize() {
    // Clear
    for (auto& row : grid)
        for (auto& cell : row)
            cell = nullptr;

    enPassantTarget      = Position(-1, -1);
    currentTurn          = Color::WHITE;
    whiteKingsideCastle  = true;
    whiteQueensideCastle = true;
    blackKingsideCastle  = true;
    blackQueensideCastle = true;

    // Black pieces (row 0 = rank 8)
    grid[0][0] = make_shared<Rook>  (Color::BLACK, Position(0,0));
    grid[0][1] = make_shared<Knight>(Color::BLACK, Position(0,1));
    grid[0][2] = make_shared<Bishop>(Color::BLACK, Position(0,2));
    grid[0][3] = make_shared<Queen> (Color::BLACK, Position(0,3));
    grid[0][4] = make_shared<King>  (Color::BLACK, Position(0,4));
    grid[0][5] = make_shared<Bishop>(Color::BLACK, Position(0,5));
    grid[0][6] = make_shared<Knight>(Color::BLACK, Position(0,6));
    grid[0][7] = make_shared<Rook>  (Color::BLACK, Position(0,7));
    for (int c = 0; c < 8; ++c)
        grid[1][c] = make_shared<Pawn>(Color::BLACK, Position(1,c));

    // White pieces (row 7 = rank 1)
    grid[7][0] = make_shared<Rook>  (Color::WHITE, Position(7,0));
    grid[7][1] = make_shared<Knight>(Color::WHITE, Position(7,1));
    grid[7][2] = make_shared<Bishop>(Color::WHITE, Position(7,2));
    grid[7][3] = make_shared<Queen> (Color::WHITE, Position(7,3));
    grid[7][4] = make_shared<King>  (Color::WHITE, Position(7,4));
    grid[7][5] = make_shared<Bishop>(Color::WHITE, Position(7,5));
    grid[7][6] = make_shared<Knight>(Color::WHITE, Position(7,6));
    grid[7][7] = make_shared<Rook>  (Color::WHITE, Position(7,7));
    for (int c = 0; c < 8; ++c)
        grid[6][c] = make_shared<Pawn>(Color::WHITE, Position(6,c));
}

// ────────────────────────────────────────────────────────────
//  Display
// ────────────────────────────────────────────────────────────
void Board::display() const {
    cout << "\n   +---+---+---+---+---+---+---+---+\n";
    for (int r = 0; r < 8; ++r) {
        cout << " " << (8-r) << " |";
        for (int c = 0; c < 8; ++c) {
            auto& piece = grid[r][c];
            char sym = piece ? piece->getSymbol() : ((r+c)%2==0?' ':' ');
            if (!piece) {
                // Checkerboard dots for empty squares
                sym = (r+c)%2==0 ? ' ' : '.';
            }
            cout << " " << sym << " |";
        }
        cout << "\n   +---+---+---+---+---+---+---+---+\n";
    }
    cout << "     a   b   c   d   e   f   g   h\n\n";
}

void Board::displayFancy() const {
    const string RESET  = "\033[0m";
    const string WHITE_BG  = "\033[47m";
    const string DARK_BG   = "\033[100m";
    const string WHITE_PC  = "\033[1;97m";
    const string BLACK_PC  = "\033[1;90m";
    const string LABEL     = "\033[1;33m";

    cout << "\n" << LABEL << "     a  b  c  d  e  f  g  h\n" << RESET;
    cout << LABEL << "   +------------------------+\n" << RESET;

    for (int r = 0; r < 8; ++r) {
        cout << LABEL << " " << (8-r) << " |" << RESET;
        for (int c = 0; c < 8; ++c) {
            bool lightSq = (r+c) % 2 == 0;
            string bg = lightSq ? WHITE_BG : DARK_BG;
            auto& piece = grid[r][c];
            if (!piece) {
                cout << bg << "   " << RESET;
            } else {
                string fg = (piece->getColor() == Color::WHITE) ? WHITE_PC : BLACK_PC;
                cout << bg << fg << " " << piece->getSymbol() << " " << RESET;
            }
        }
        cout << LABEL << "|" << (8-r) << RESET << "\n";
    }
    cout << LABEL << "   +------------------------+\n";
    cout << "     a  b  c  d  e  f  g  h\n\n" << RESET;
}

// ────────────────────────────────────────────────────────────
//  Getters / setters
// ────────────────────────────────────────────────────────────
shared_ptr<Piece> Board::getPiece(Position p) const {
    if (!p.isValid()) return nullptr;
    return grid[p.row][p.col];
}
shared_ptr<Piece> Board::getPiece(int row, int col) const {
    return getPiece(Position(row, col));
}
void Board::setPiece(Position p, shared_ptr<Piece> piece) {
    if (p.isValid()) grid[p.row][p.col] = piece;
}
void Board::clearSquare(Position p) {
    if (p.isValid()) grid[p.row][p.col] = nullptr;
}
bool Board::isOccupied(Position p) const {
    return p.isValid() && grid[p.row][p.col] != nullptr;
}
bool Board::isOccupiedByColor(Position p, Color c) const {
    auto pc = getPiece(p);
    return pc && pc->getColor() == c;
}
bool Board::isEmpty(Position p) const {
    return !isOccupied(p);
}

// ────────────────────────────────────────────────────────────
//  Deep clone (for AI look-ahead)
// ────────────────────────────────────────────────────────────
Board Board::clone() const {
    Board b;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (grid[r][c])
                b.grid[r][c] = grid[r][c]->clone();
    b.enPassantTarget      = enPassantTarget;
    b.currentTurn          = currentTurn;
    b.whiteKingsideCastle  = whiteKingsideCastle;
    b.whiteQueensideCastle = whiteQueensideCastle;
    b.blackKingsideCastle  = blackKingsideCastle;
    b.blackQueensideCastle = blackQueensideCastle;
    return b;
}

// ────────────────────────────────────────────────────────────
//  Apply Move
// ────────────────────────────────────────────────────────────
void Board::applyMove(const Move& mv) {
    auto piece = getPiece(mv.from);
    if (!piece) return;

    // Reset en-passant
    Position prevEP = enPassantTarget;
    enPassantTarget = Position(-1, -1);

    // Castling
    if (mv.isCastling) {
        applyCastling(mv);
        piece->setHasMoved(true);
        currentTurn = (currentTurn == Color::WHITE) ? Color::BLACK : Color::WHITE;
        return;
    }

    // En-passant capture
    if (mv.isEnPassant) {
        applyEnPassant(mv);
        piece->setHasMoved(true);
        currentTurn = (currentTurn == Color::WHITE) ? Color::BLACK : Color::WHITE;
        return;
    }

    // Normal / capture
    clearSquare(mv.from);
    piece->setPosition(mv.to);
    piece->setHasMoved(true);
    setPiece(mv.to, piece);

    // Pawn double push → set en-passant target
    if (piece->getType() == PieceType::PAWN) {
        int dr = mv.to.row - mv.from.row;
        if (abs(dr) == 2)
            enPassantTarget = Position((mv.from.row + mv.to.row) / 2, mv.from.col);
    }

    // Promotion
    if (mv.isPromotion)
        applyPromotion(mv);

    // Update castling rights
    if (piece->getType() == PieceType::KING) {
        if (piece->getColor() == Color::WHITE) { whiteKingsideCastle = false; whiteQueensideCastle = false; }
        else                                   { blackKingsideCastle = false; blackQueensideCastle = false; }
    }
    if (piece->getType() == PieceType::ROOK) {
        if (mv.from == Position(7,0)) whiteQueensideCastle = false;
        if (mv.from == Position(7,7)) whiteKingsideCastle  = false;
        if (mv.from == Position(0,0)) blackQueensideCastle = false;
        if (mv.from == Position(0,7)) blackKingsideCastle  = false;
    }

    currentTurn = (currentTurn == Color::WHITE) ? Color::BLACK : Color::WHITE;
}

void Board::applyCastling(const Move& mv) {
    auto king = getPiece(mv.from);
    clearSquare(mv.from);

    bool kingside = (mv.to.col == 6);
    int  rookFromCol = kingside ? 7 : 0;
    int  rookToCol   = kingside ? 5 : 3;
    int  row = mv.from.row;

    auto rook = getPiece(Position(row, rookFromCol));
    clearSquare(Position(row, rookFromCol));

    king->setPosition(mv.to);
    setPiece(mv.to, king);

    rook->setPosition(Position(row, rookToCol));
    setPiece(Position(row, rookToCol), rook);
    rook->setHasMoved(true);

    if (mv.from.row == 7) { whiteKingsideCastle = false; whiteQueensideCastle = false; }
    else                  { blackKingsideCastle = false; blackQueensideCastle = false; }
}

void Board::applyEnPassant(const Move& mv) {
    auto pawn = getPiece(mv.from);
    clearSquare(mv.from);
    pawn->setPosition(mv.to);
    setPiece(mv.to, pawn);

    // Remove captured pawn
    int captureRow = mv.from.row;
    clearSquare(Position(captureRow, mv.to.col));
    enPassantTarget = Position(-1,-1);
}

void Board::applyPromotion(const Move& mv) {
    auto& sq = grid[mv.to.row][mv.to.col];
    Color c  = sq->getColor();
    switch (mv.promoteTo) {
        case PieceType::QUEEN:  sq = make_shared<Queen> (c, mv.to); break;
        case PieceType::ROOK:   sq = make_shared<Rook>  (c, mv.to); break;
        case PieceType::BISHOP: sq = make_shared<Bishop>(c, mv.to); break;
        case PieceType::KNIGHT: sq = make_shared<Knight>(c, mv.to); break;
        default: break;
    }
    sq->setHasMoved(true);
}

// ────────────────────────────────────────────────────────────
//  Attack / check detection
// ────────────────────────────────────────────────────────────
bool Board::isSquareAttacked(Position sq, Color byColor) const {
    // Scan all opponent pieces and see if they can move to sq
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            auto& pc = grid[r][c];
            if (!pc || pc->getColor() != byColor) continue;
            // For pawns, attack diagonals specifically (not forward push)
            if (pc->getType() == PieceType::PAWN) {
                int dir = (byColor == Color::WHITE) ? -1 : 1;
                if (Position(r+dir, c-1) == sq || Position(r+dir, c+1) == sq)
                    return true;
                continue;
            }
            auto moves = pc->getLegalMoves(*this);
            for (auto& m : moves)
                if (m == sq) return true;
        }
    return false;
}

bool Board::isInCheck(Color color) const {
    Position king = findKing(color);
    if (!king.isValid()) return false;
    Color opp = (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    return isSquareAttacked(king, opp);
}

Position Board::findKing(Color color) const {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            auto& pc = grid[r][c];
            if (pc && pc->getColor() == color && pc->getType() == PieceType::KING)
                return Position(r,c);
        }
    return Position(-1,-1);
}

// ────────────────────────────────────────────────────────────
//  Legal moves for all pieces of a color (filters check)
// ────────────────────────────────────────────────────────────
vector<Move> Board::getAllLegalMoves(Color color) const {
    vector<Move> legal;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            auto& pc = grid[r][c];
            if (!pc || pc->getColor() != color) continue;
            auto targets = pc->getLegalMoves(*this);
            for (auto& to : targets) {
                Move mv(Position(r,c), to);

                // Detect en-passant
                if (pc->getType() == PieceType::PAWN && to == enPassantTarget)
                    mv.isEnPassant = true;

                // Detect promotion
                if (pc->getType() == PieceType::PAWN && (to.row == 0 || to.row == 7))
                    mv.isPromotion = true;

                // Detect castling
                if (pc->getType() == PieceType::KING && abs(to.col - c) == 2)
                    mv.isCastling = true;

                // Filter: does this move leave our king in check?
                Board copy = this->clone();
                copy.applyMove(mv);
                // After move, it's opponent's turn; check if OUR king is in check
                if (!copy.isInCheck(color))
                    legal.push_back(mv);
            }
        }
    return legal;
}

bool Board::isCheckmate(Color color) const {
    return isInCheck(color) && getAllLegalMoves(color).empty();
}
bool Board::isStalemate(Color color) const {
    return !isInCheck(color) && getAllLegalMoves(color).empty();
}
