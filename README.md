MINESWEEPER

This is a terminal version of the classic Minesweeper game made in C using curses.

The game runs entirely in the console and supports mouse input for revealing cells and placing flags.

FEATURES

Multiple difficulty levels (easy, medium, hard, expert)
Random mine placement
Recursive reveal of empty cells
Flag system
Win and lose detection
Mouse support
Simple menu interface

CONTROLS

Left mouse button: reveal cell
Right mouse button: place/remove flag
Menu navigation: arrow keys + enter

HOW TO RUN

Compile the project with a C compiler that supports curses.

Example (Linux):
gcc main.c menu.c play.c -o minesweeper -lncurses

Then run:
./minesweeper

On Windows, you need a curses-compatible environment (like PDCurses).
If you already have PDCurses installed, you can just run the compile.bat

STRUCTURE

main.c: program entry and main loop
menu.c: menu system and level selection
play.c: game logic (grid, mines, input, win/lose)
map.h: map definitions and difficulty settings

WHAT I LEARNED

Working with dynamic memory (2D grids)
Handling input in terminal (including mouse)
Structuring a game loop
Managing state (win, lose, grid updates)
Separating logic, interface and control flow

LIMITATIONS

This project was developed mainly for learning purposes, so there are some known limitations and simplifications:

Code structure could be improved and further modularized
Some edge cases and polish are missing
Focus was more on implementing core mechanics than refining the overall experience
