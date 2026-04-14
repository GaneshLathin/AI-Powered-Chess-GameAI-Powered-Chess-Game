# AI-Powered Chess Project: Interview Prep Guide

## 📌 Project Overview
This is a fully-functional **AI-Powered Chess Game** written in C++. It allows a user to play chess against an AI (Artificial Intelligence) directly in the console terminal. 

The project stands out because it uses **Object-Oriented Programming (OOP)** (classes, objects, polymorphism) to represent the game structure and uses a classic AI algorithm called **Minimax with Alpha-Beta Pruning** to make smart chess moves.

---

## 🏗️ Code Structure: What does each file do?

If an interviewer asks you what is inside your project, you can break it down into these four core modules:

### 1. `main.cpp` (The Entry Point)
- **Purpose:** This is where the program starts when you run it.
- **What it does:** It enables special text colors for the terminal and creates an instance of the `Game` class to start the play loop. It’s very short and neat!

### 2. `Game.h` / `Game.cpp` (The Game Loop)
- **Purpose:** Controls the flow of the game, like a referee or a host.
- **What it does:** 
  - Asks the user to choose difficulty, game mode (Player vs AI, Player vs Player), and color.
  - Takes turns passing the control between the Human Player and the AI.
  - Checks user inputs (like "e2 e4") to see if the move is in the right format.
  - Checks if the game has ended in Checkmate, Stalemate, or if a player resigned.

### 3. `Board.h` / `Board.cpp` (The Chessboard Memory)
- **Purpose:** Manages the actual 8x8 grid of the chessboard.
- **What it does:**
  - Uses an 8x8 `std::array` grid to hold all the pieces.
  - Has logic to move pieces around (`applyMove`).
  - Keeps track of whose turn it is, and special rules like Castling and En Passant.
  - **Most Importantly:** It has functions to check for Checkmate and to generate a list of all legal moves (`getAllLegalMoves`) that a player can make right now.

### 4. `Piece.h` / `Pieces.h` / `Pieces.cpp` (The Chess Pieces)
- **Purpose:** Defines how each individual piece (Pawn, Knight, Rook, etc.) behaves.
- **What it does:**
  - **`Piece.h`** creates a bare-bones "Abstract Base Class". It says every piece must have a color, a position, and a function called `getLegalMoves()`. It's a template for all pieces.
  - **`Pieces.cpp`** creates the specific pieces (e.g., `Pawn`, `Knight`, `Queen`). Each specific piece writes its own logic for `getLegalMoves()` because a Knight moves in an 'L' shape and a Rook moves in straight lines.

### 5. `AI.h` / `AI.cpp` (The Brain of the Game)
- **Purpose:** Looks at the board and calculates the best possible move.
- **How it works:** It uses an algorithm called **Minimax** combined with an optimization called **Alpha-Beta Pruning**. 
  - It generates all possible moves, plays out hypothetical games several moves into the future, and chooses the one that gives it the biggest advantage.

---

## 🧠 Explain the AI Algorithm (Minimax + Alpha-Beta Pruning)

This is the most common interview question for this type of project! Here is how to explain it in simple English:

### How does Minimax work?
> *"Minimax is an algorithm where the AI tries to look into the future by simulating all possible moves. Since this is a two-player game, the AI assumes that **I am trying to MAXimize my score**, while **my opponent is trying to MINimize my score**."*

- **The Evaluation:** The AI gives the board a "score". Example: A Queen is worth 9 points, a Rook is 5, a Pawn is 1. If the AI has more pieces on the board than the player, the score is high (good for AI). If the player has more pieces, the score is low (bad for AI).
- **The Tree:** The AI imagines a "tree" of moves. If the AI is set to difficulty level 3 (Depth 3), it looks 3 moves ahead into the future: "If I do this, what will the human do? And then what will I do?" 
- It evaluates the board at the end of every scenario and picks the path that gives it the best guaranteed score, assuming the human plays perfectly.

### What is Alpha-Beta Pruning?
> *"Alpha-Beta Pruning is an upgrade to Minimax. It is basically a shortcut. It helps the AI skip evaluating moves that we already know are bad, making it run much faster."*

- **Simple Analogy:** Imagine you are shopping for a car. You find a great car for $10,000. Then you go to a second dealership, and the first car they show you is an identical car for $20,000. Do you need to look at the rest of the cars at the second dealership? No! You know immediately it's a bad deal. 
- That is **Pruning**. Alpha-Beta keeps track of the best score found so far. If a branch of the move-tree looks worse than a move we already found elsewhere, it stops searching that branch to save time.

---

## 💻 Key OOP Concepts Used in the Project

Interviewers love testing your C++ Object-Oriented knowledge. Here is how your project uses those exact concepts:

1. **Encapsulation (Hiding data):**
   - We don't let external code change a piece's location directly. They have to use functions in the `Board` or `Game` class. We hide internal logic by using `private` and `protected` keywords.

2. **Inheritance (Family of Classes):**
   - The `Piece` class is the "parent". The `King`, `Queen`, `Rook`, etc. are "children" who inherit everything from the parent (like the `Color` and `Position` properties) so we don't have to rewrite the same code for every piece.

3. **Polymorphism (Many forms):**
   - The `Piece` class has a "virtual" function called `getLegalMoves()`. 
   - Because of polymorphism, the `Board` can tell *any* piece to "get legal moves", and C++ automatically knows whether to run the Pawn's move logic or the Knight's move logic depending on what piece is actually sitting on the square.

---

## 🎯 How to use this guide in an interview:

* **“Tell me about your project.”** -> Start with the **Project Overview**, mention C++ and the Minimax AI. 
* **”How does the game know if a move is legal?”** -> Mention the `Pieces.cpp` file and how each piece uses **Polymorphism** to define its own movement logic.
* **”How did you build the AI?”** -> Explain **Minimax** as a way of simulating future moves, and **Alpha-Beta Pruning** as a shopping shortcut to save computer time.
