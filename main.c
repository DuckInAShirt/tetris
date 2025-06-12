#include "tetris.h"
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
    Tetromino *current_piece = malloc(sizeof(Tetromino));
    memcpy(current_piece->shape, TETROMINO_SHAPES[num], sizeof(current_piece->shape));
    current_piece->size = TETROMINO_SIZES[num];
    // 3. Set initial position (top center)
    current_piece-> x = COLS / 2 - current_piece->size / 2;
    current_piece->y = 0;
}

bool check_collision(Tetromino piece) {
    // TODO: Implement collision detection
    // Remember to check for 3 conditions:
    // 1. Out of left/right bounds
    // 2. Out of bottom bound
    // 3. Overlapping with existing blocks in playfield
    return false; // Placeholder
}

void rotate_piece(Tetromino *piece) {
    // TODO: Implement rotation logic
    // Tip: Use a temporary 4x4 array. Transpose, then reverse rows.
}

void handle_input() {
    // TODO: Implement input handling
    // 1. Get char with getch_nonblock()
    // 2. Create a temporary copy of current_piece
    // 3. Modify the copy based on input ('a', 'd', 's', 'w')
    // 4. If the new position is valid (check_collision), update current_piece
}

void lock_piece() {
    // TODO: "Bake" the current piece into the playfield
    // Iterate through the piece's shape and update the playfield grid
}

void clear_lines() {
    // TODO: Check for and clear any full lines
    // 1. Iterate from bottom row to top
    // 2. If a line is full, increment score
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
