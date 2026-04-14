cmake_minimum_required(VERSION 3.16)
project(ChessAI VERSION 1.0.0 LANGUAGES CXX)

# ── C++17 standard ──────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Source files ─────────────────────────────────────────────
set(SOURCES
    src/main.cpp
    src/Board.cpp
    src/Pieces.cpp
    src/AI.cpp
    src/Game.cpp
)

# ── Include directories ──────────────────────────────────────
include_directories(include)

# ── Executable ───────────────────────────────────────────────
add_executable(chess ${SOURCES})

# ── Compiler flags ───────────────────────────────────────────
if(MSVC)
    target_compile_options(chess PRIVATE /W4 /O2)
else()
    target_compile_options(chess PRIVATE -Wall -Wextra -O2)
endif()

# ── Release / Debug configs ──────────────────────────────────
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

message(STATUS "")
message(STATUS "  ♟  AI Chess Game — CMake Configuration")
message(STATUS "  Compiler : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Standard : C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Dir: ${CMAKE_BINARY_DIR}")
message(STATUS "")
