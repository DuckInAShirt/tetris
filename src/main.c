#include "tetris.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// --- Global Variable Definitions ---
int playfield[ROWS][COLS] = {0};
Tetromino current_piece;
int score = 0;
bool game_over = false;

// Shape templates for all 7 tetrominoes (I, O, T, S, Z, J, L)
// Using a 4x4 grid for easier rotation
const int TETROMINO_SHAPES[7][4][4] = {
    {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}, // I
    {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}}, // O
    {{0,0,0,0}, {0,1,0,0}, {1,1,1,0}, {0,0,0,0}}, // T
    {{0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0}}, // S
    {{0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0}}, // Z
    {{0,0,0,0}, {0,0,1,0}, {1,1,1,0}, {0,0,0,0}}, // J
    {{0,0,0,0}, {1,0,0,0}, {1,1,1,0}, {0,0,0,0}}  // L
};
const int TETROMINO_SIZES[7] = {4, 2, 3, 3, 3, 3, 3}; // Size of the shape matrix


// --- Terminal Control (Provided for you) ---
static struct termios old_tio, new_tio;

void init_termios() {
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void reset_termios() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
}

char getch_nonblock() {
    char ch = 0;
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
        read(STDIN_FILENO, &ch, 1);
    }
    return ch;
}

// --- Game Functions (Your implementation goes here!) ---

void generate_new_piece() {
    // TODO: Implement piece generation logic
    // 1. Pick a random shape index (0-6)
    int num = rand() % 7;
    // 2. Copy the shape and size from the global templates
    memcpy(current_piece.shape, TETROMINO_SHAPES[num], sizeof(current_piece.shape));
    current_piece.size = TETROMINO_SIZES[num];
    // 3. Set initial position (top center)
    current_piece.x = COLS / 2 - current_piece.size / 2;
    current_piece.y = 0;
}

bool check_collision(Tetromino piece) {
    // TODO: Implement collision detection
    // Remember to check for 3 conditions:
    // 1. Out of left/right bounds
    if (piece.x >= ROWS || piece.x < 0) {
        return true;
    }
    // 2. Out of bottom bound
    if (piece.y >= COLS) {
        return true;
    }
    // 3. Overlapping with existing blocks in playfield
    if (piece.y >= 0 && playfield[piece.x][piece.y] != 0) {
        return true;
    }
    return false; // Placeholder
}

void rotate_piece(Tetromino *piece) {
    // TODO: Implement rotation logic
    // Tip: Use a temporary 4x4 array. Transpose, then reverse rows.
    int n = 4;
    for (int i = 0; i < n / 2; i += 1) {
        for (int j = 0; j < (n + 1) / 2; j += 1) {  
            int t = piece->shape[i][j];
            piece->shape[i][j] = piece->shape[j][n - 1 - i];
            piece->shape[j][n - 1 - i] = piece->shape[n - 1 - i][n - 1 - j];
            piece->shape[n - 1 - i][n - 1 - j] = piece->shape[n - 1 - j][i];
            piece->shape[n - 1 - j][i] = t;
        }
    }
}

void handle_input() {
    // TODO: Implement input handling
    // 1. Get char with getch_nonblock()
    char op = getch_nonblock();
    // 2. Create a temporary copy of current_piece
    Tetromino copy = current_piece;
    // 3. Modify the copy based on input ('a', 'd', 's', 'w')
    switch (op) {
        case 'a':
            copy.x--;
            break;
        case 'd':
            copy.x++;
            break;
        case 's':
            copy.y++;
            break;
        case 'w':
            rotate_piece(&copy);
            break;
        default:
            return;
    }
    // 4. If the new position is valid (check_collision), update current_piece
    if (!check_collision(copy)) {
        current_piece = copy;
    }
}

void lock_piece() {
    // TODO: "Bake" the current piece into the playfield
    // Iterate through the piece's shape and update the playfield grid
    for (int i = 0; i < 4; i += 1) {
        for (int j = 0; j < 4; j += 1) {
            if (current_piece.shape[i][j] == 1) {
                playfield[current_piece.x + i][current_piece.y + j] = 1;
            }
        }
    }
}

void clear_lines() {
    // TODO: Check for and clear any full lines
    // 1. Iterate from bottom row to top
    int cnt = 0;
    int start = -1;
    for (int i = ROWS - 1; i >= 0; i -= 1) {
        bool flag = true;
        for (int j = 0; j < COLS; j += 1) {
            if (playfield[i][j] == 0) {
                flag = false;
                break;
            }
        }
        if (flag) {
            start = i > start ? i : start; 
            cnt++;
        }
    }
    // 2. If a line is full, increment score
    switch (cnt) {
        case 1:
            score += 100;
            break;
        case 2:
            score += 200;
            break;
        case 3:
            score += 400;
            break;
        case 4:
            score += 800;
            break;
    }
    // 3. Shift all rows above it down
}

void draw_game() {
    // TODO: Implement screen drawing
    // 1. Use ANSI escape codes to clear screen: printf("\x1b[H\x1b[2J");
    // 2. Create a temporary display buffer
    // 3. Copy playfield to buffer, then draw current_piece onto buffer
    // 4. Print the buffer with borders
    // 5. Print score and other info
    // 6. fflush(stdout)
}

void init_game() {
    srand(time(NULL));
    memset(playfield, 0, sizeof(playfield));
    score = 0;
    game_over = false;
    generate_new_piece();
}

// --- Main Game Loop ---
int main() {
    init_termios();
    init_game();

    clock_t last_fall_time = clock();
    double fall_interval = 0.8; // seconds

    while (!game_over) {
        handle_input();

        clock_t current_time = clock();
        if ((double)(current_time - last_fall_time) / CLOCKS_PER_SEC > fall_interval) {
            Tetromino temp = current_piece;
            temp.y++;

            if (check_collision(temp)) {
                lock_piece();
                clear_lines();
                generate_new_piece();
                if (check_collision(current_piece)) {
                    game_over = true;
                }
            } else {
                current_piece = temp;
            }
            last_fall_time = clock();
        }

        draw_game();
        usleep(16000); // Sleep for ~16ms to yield CPU (~60 FPS cap)
    }

    reset_termios();
    printf("Game Over! Final Score: %d\n", score);

    return 0;
}
