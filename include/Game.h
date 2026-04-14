#pragma once
#include "Board.h"
#include "AI.h"
#include <string>


using namespace std;


// ============================================================
//  Game.h  –  Game controller / state machine
//  OOP Concepts: Encapsulation, Single Responsibility
// ============================================================

enum class GameMode {
    PLAYER_VS_PLAYER,
    PLAYER_VS_AI,
    AI_VS_AI
};

enum class GameState {
    RUNNING,
    CHECKMATE,
    STALEMATE,
    DRAW,
    RESIGNED
};

class Game {
public:
    Game();

    void run();              // Main game loop
    void printWelcome() const;
    void printHelp()    const;

private:
    Board     board_;
    ChessAI   ai_;
    GameMode  mode_;
    GameState state_;
    Color     humanColor_;   // Which side the human plays (PLAYER_VS_AI)
    int       moveCount_;

    // Turn management
    void playerTurn();
    void aiTurn();

    // Input parsing: "e2 e4"  →  Move
    bool parseMove(const string& input, Move& mv) const;
    Position parseSquare(const string& sq) const; // "e4" → Position(3,4)

    // Post-move checks
    void checkGameEnd();
    bool isGameOver() const { return state_ != GameState::RUNNING; }

    // Setup
    void setupGame();
    void selectDifficulty();
    static string colorName(Color c);
};
