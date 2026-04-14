#include "../include/Game.h"
#include "../include/Pieces.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <thread>
#ifdef _WIN32
  #include <windows.h>
#endif
#include <thread>


using namespace std;


// ============================================================
//  Game.cpp  –  Game loop, player input, AI integration
// ============================================================

// ANSI colour helpers
namespace CLR {
    const string RESET   = "\033[0m";
    const string BOLD    = "\033[1m";
    const string RED     = "\033[1;31m";
    const string GREEN   = "\033[1;32m";
    const string YELLOW  = "\033[1;33m";
    const string CYAN    = "\033[1;36m";
    const string MAGENTA = "\033[1;35m";
    const string WHITE   = "\033[1;97m";
    const string BLUE    = "\033[1;34m";
}

Game::Game()
    : ai_(4), mode_(GameMode::PLAYER_VS_AI),
      state_(GameState::RUNNING), humanColor_(Color::WHITE), moveCount_(0)
{
    board_.initialize();
}

// ────────────────────────────────────────────────────────────
//  Welcome Banner
// ────────────────────────────────────────────────────────────
void Game::printWelcome() const {
    cout << CLR::CYAN;
    cout << R"(
  ╔══════════════════════════════════════════════════════════╗
  ║                                                          ║
  ║         ♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜   AI-POWERED CHESS   ♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖  ║
  ║                                                          ║
  ║        Minimax Engine with Alpha-Beta Pruning            ║
  ║        Object-Oriented Design  |  C++17                  ║
  ║                                                          ║
  ╚══════════════════════════════════════════════════════════╝
)" << CLR::RESET;
}

void Game::printHelp() const {
    cout << CLR::YELLOW << "\n  HOW TO PLAY:\n" << CLR::RESET;
    cout << "  Enter moves in algebraic notation: " << CLR::GREEN << "e2 e4\n" << CLR::RESET;
    cout << "  Special commands:\n";
    cout << "    " << CLR::CYAN << "help" << CLR::RESET << "       – Show this help\n";
    cout << "    " << CLR::CYAN << "board" << CLR::RESET << "      – Redraw the board\n";
    cout << "    " << CLR::CYAN << "moves" << CLR::RESET << "      – List all legal moves\n";
    cout << "    " << CLR::CYAN << "resign" << CLR::RESET << "     – Resign the game\n";
    cout << "    " << CLR::CYAN << "quit" << CLR::RESET << "       – Exit the program\n\n";
}

// ────────────────────────────────────────────────────────────
//  Parse algebraic square: "e4" → Position(4, 4)
// ────────────────────────────────────────────────────────────
Position Game::parseSquare(const string& sq) const {
    if (sq.size() < 2) return Position(-1,-1);
    char file = tolower(sq[0]);
    char rank = sq[1];
    if (file < 'a' || file > 'h') return Position(-1,-1);
    if (rank < '1' || rank > '8') return Position(-1,-1);
    int col = file - 'a';          // a=0 … h=7
    int row = '8' - rank;          // '8'→0, '1'→7
    return Position(row, col);
}

bool Game::parseMove(const string& input, Move& mv) const {
    istringstream ss(input);
    string fromStr, toStr;
    ss >> fromStr >> toStr;
    if (fromStr.empty() || toStr.empty()) return false;

    Position from = parseSquare(fromStr);
    Position to   = parseSquare(toStr);
    if (!from.isValid() || !to.isValid()) return false;

    mv = Move(from, to);

    // Check promotion flag
    auto pc = board_.getPiece(from);
    if (pc && pc->getType() == PieceType::PAWN) {
        if ((pc->isWhite() && to.row == 0) || (pc->isBlack() && to.row == 7)) {
            mv.isPromotion = true;
            mv.promoteTo   = PieceType::QUEEN; // auto-promote to queen
        }
    }
    return true;
}

// ────────────────────────────────────────────────────────────
//  Game Setup
// ────────────────────────────────────────────────────────────
void Game::selectDifficulty() {
    cout << CLR::YELLOW << "\n  Select Difficulty:\n" << CLR::RESET;
    cout << "  [1] Easy    (depth 2)\n";
    cout << "  [2] Medium  (depth 3)\n";
    cout << "  [3] Hard    (depth 4)\n";
    cout << "  [4] Expert  (depth 5 – slow!)\n";
    cout << CLR::CYAN << "  Your choice: " << CLR::RESET;

    int choice = 2;
    string line;
    getline(cin, line);
    if (!line.empty()) choice = stoi(line);

    int dep = 3;
    switch(choice) {
        case 1: dep = 2; break;
        case 2: dep = 3; break;
        case 3: dep = 4; break;
        case 4: dep = 5; break;
        default: dep = 3;
    }
    ai_.setDepth(dep);
    cout << CLR::GREEN << "  Depth set to " << dep << "\n" << CLR::RESET;
}

void Game::setupGame() {
    cout << CLR::YELLOW << "\n  Game Mode:\n" << CLR::RESET;
    cout << "  [1] Player vs AI\n";
    cout << "  [2] Player vs Player\n";
    cout << "  [3] AI vs AI    (watch the AI play itself!)\n";
    cout << CLR::CYAN << "  Your choice: " << CLR::RESET;

    string line;
    getline(cin, line);
    int choice = line.empty() ? 1 : stoi(line);

    switch (choice) {
        case 2: mode_ = GameMode::PLAYER_VS_PLAYER; break;
        case 3: mode_ = GameMode::AI_VS_AI;         break;
        default: mode_ = GameMode::PLAYER_VS_AI;
    }

    if (mode_ == GameMode::PLAYER_VS_AI) {
        cout << "\n  Play as:\n";
        cout << "  [1] White (moves first)\n";
        cout << "  [2] Black\n";
        cout << CLR::CYAN << "  Your choice: " << CLR::RESET;
        getline(cin, line);
        humanColor_ = (line == "2") ? Color::BLACK : Color::WHITE;
        selectDifficulty();
    } else if (mode_ == GameMode::AI_VS_AI) {
        selectDifficulty();
    }

    board_.initialize();
    moveCount_ = 0;
    state_     = GameState::RUNNING;
}

// ────────────────────────────────────────────────────────────
//  Player Turn
// ────────────────────────────────────────────────────────────
void Game::playerTurn() {
    Color turn = board_.currentTurn;
    string turnStr = colorName(turn);

    if (board_.isInCheck(turn))
        cout << CLR::RED << "  ⚠  " << turnStr << " is in CHECK!\n" << CLR::RESET;

    cout << CLR::BOLD << "\n  " << turnStr << "'s move (e.g. e2 e4): " << CLR::RESET;

    string input;
    getline(cin, input);
    // trim
    input.erase(0, input.find_first_not_of(" \t"));
    input.erase(input.find_last_not_of(" \t")+1);

    if (input == "quit" || input == "exit") {
        state_ = GameState::RESIGNED;
        return;
    }
    if (input == "resign") {
        state_ = GameState::RESIGNED;
        cout << CLR::YELLOW << "  " << turnStr << " resigns!\n" << CLR::RESET;
        return;
    }
    if (input == "help")  { printHelp();  playerTurn(); return; }
    if (input == "board") { board_.displayFancy(); playerTurn(); return; }

    if (input == "moves") {
        auto legal = board_.getAllLegalMoves(turn);
        cout << CLR::CYAN << "  Legal moves (" << legal.size() << "):\n" << CLR::RESET;
        for (auto& mv : legal) {
            char fromFile = 'a' + mv.from.col;
            char fromRank = '8' - mv.from.row;
            char toFile   = 'a' + mv.to.col;
            char toRank   = '8' - mv.to.row;
            cout << "    " << fromFile << fromRank << " → " << toFile << toRank << "\n";
        }
        playerTurn();
        return;
    }

    Move mv;
    if (!parseMove(input, mv)) {
        cout << CLR::RED << "  ✗ Invalid format. Try: e2 e4\n" << CLR::RESET;
        playerTurn();
        return;
    }

    // Check move is legal
    auto legal = board_.getAllLegalMoves(turn);
    bool found = false;
    for (auto& lm : legal) {
        if (lm.from == mv.from && lm.to == mv.to) {
            mv = lm;   // use the fully-populated legal move (flags set)
            found = true;
            break;
        }
    }

    if (!found) {
        cout << CLR::RED << "  ✗ Illegal move. Try again.\n" << CLR::RESET;
        playerTurn();
        return;
    }

    board_.applyMove(mv);
    ++moveCount_;

    char fromFile = 'a' + mv.from.col;
    char fromRank = '8' - mv.from.row;
    char toFile   = 'a' + mv.to.col;
    char toRank   = '8' - mv.to.row;
    cout << CLR::GREEN << "  ✔  " << turnStr << " played "
              << fromFile << fromRank << " → " << toFile << toRank << "\n" << CLR::RESET;
}

// ────────────────────────────────────────────────────────────
//  AI Turn
// ────────────────────────────────────────────────────────────
void Game::aiTurn() {
    Color turn = board_.currentTurn;
    cout << CLR::MAGENTA << "\n  🤖 AI (" << colorName(turn) << ") is thinking...\n" << CLR::RESET;

    auto start = chrono::high_resolution_clock::now();
    Move best  = ai_.getBestMove(board_);
    auto end   = chrono::high_resolution_clock::now();
    double ms  = chrono::duration<double,milli>(end-start).count();

    if (!best.from.isValid()) {
        cout << CLR::RED << "  AI has no moves!\n" << CLR::RESET;
        return;
    }

    board_.applyMove(best);
    ++moveCount_;

    char fromFile = 'a' + best.from.col;
    char fromRank = '8' - best.from.row;
    char toFile   = 'a' + best.to.col;
    char toRank   = '8' - best.to.row;

    cout << CLR::MAGENTA << "  🤖 AI played: "
              << fromFile << fromRank << " → " << toFile << toRank
              << "  (" << ai_.getNodesExplored() << " nodes, "
              << static_cast<int>(ms) << " ms)\n" << CLR::RESET;

    // Brief pause so AI vs AI is watchable
    if (mode_ == GameMode::AI_VS_AI) {
#ifdef _WIN32
        Sleep(400);
#else
        this_thread::sleep_for(chrono::milliseconds(400));
#endif
    }
}

// ────────────────────────────────────────────────────────────
//  Post-Move: check for checkmate / stalemate
// ────────────────────────────────────────────────────────────
void Game::checkGameEnd() {
    Color turn = board_.currentTurn;
    if (board_.isCheckmate(turn)) {
        board_.displayFancy();
        Color winner = (turn == Color::WHITE) ? Color::BLACK : Color::WHITE;
        cout << CLR::RED << "\n  ♟  CHECKMATE!  " << colorName(winner)
                  << " wins the game! 🎉\n" << CLR::RESET;
        state_ = GameState::CHECKMATE;
    } else if (board_.isStalemate(turn)) {
        board_.displayFancy();
        cout << CLR::YELLOW << "\n  ½  STALEMATE – It's a draw!\n" << CLR::RESET;
        state_ = GameState::STALEMATE;
    }
}

// ────────────────────────────────────────────────────────────
//  Main Game Loop
// ────────────────────────────────────────────────────────────
void Game::run() {
    printWelcome();
    printHelp();
    setupGame();

    while (!isGameOver()) {
        board_.displayFancy();
        cout << CLR::YELLOW << "  Move #" << (moveCount_/2 + 1)
                  << "  |  Turn: " << CLR::BOLD << colorName(board_.currentTurn)
                  << CLR::RESET << "\n";

        Color turn = board_.currentTurn;
        bool humanTurn = (mode_ == GameMode::PLAYER_VS_PLAYER) ||
                         (mode_ == GameMode::PLAYER_VS_AI && turn == humanColor_);

        if (humanTurn)
            playerTurn();
        else
            aiTurn();

        if (!isGameOver())
            checkGameEnd();
    }

    if (state_ != GameState::CHECKMATE && state_ != GameState::STALEMATE) {
        cout << CLR::YELLOW << "\n  Game ended. Thanks for playing!\n" << CLR::RESET;
    }

    cout << CLR::CYAN << "\n  Total moves played: " << moveCount_ << "\n" << CLR::RESET;

    // Offer new game
    cout << "\n  Play again? (y/n): ";
    string ans;
    getline(cin, ans);
    if (!ans.empty() && tolower(ans[0]) == 'y') {
        state_ = GameState::RUNNING;
        run();
    }
}

string Game::colorName(Color c) {
    return c == Color::WHITE ? "White" : "Black";
}
