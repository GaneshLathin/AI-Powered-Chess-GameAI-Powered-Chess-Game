#include "../include/Game.h"
#include <iostream>

#ifdef _WIN32
  #include <windows.h>


using namespace std;

  #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
  #endif
#endif

// ============================================================
//  main.cpp  –  Entry point
//  AI-Powered Chess  |  C++17  |  OOP + Minimax + Alpha-Beta
// ============================================================

int main() {

#ifdef _WIN32
    // Enable UTF-8 output
    SetConsoleOutputCP(65001);
    // Enable ANSI virtual terminal sequences (Windows 10+)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif

    Game game;
    game.run();
    return 0;
}
