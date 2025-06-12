#define _DEFAULT_SOURCE

#ifndef TETRIS_H
#define TETRIS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

// --- 新增的头文件 ---
#include <sys/time.h>
#include <sys/types.h>
// --- 修复结束 ---

// Game area dimensions
#define ROWS 20
#define COLS 10

// Data structure for a Tetris piece
typedef struct {
    int shape[4][4];
    int x, y;
    int size;
} Tetromino;

// --- Global Variables (declared here, defined in main.c) ---
extern int playfield[ROWS][COLS];
extern Tetromino current_piece;
extern int score;
extern bool game_over;

// --- Function Prototypes ---

// Terminal control
void init_termios();
void reset_termios();
char getch_nonblock();

// Game state
void init_game();
void generate_new_piece();

// Core logic
bool check_collision(Tetromino piece);
void lock_piece();
void clear_lines();
void rotate_piece(Tetromino *piece);

// Input and Rendering
void handle_input();
void draw_game();

#endif // TETRIS_H

