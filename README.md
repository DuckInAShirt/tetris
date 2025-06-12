# C语言俄罗斯方块开发技术指南 (WSL/Linux环境)

## 1. 项目目标
本项目旨在通过C语言，在Linux终端环境下，从零开始实现一个功能完整的俄罗斯方块游戏。你将学习并实践到：

*   **模块化编程**：通过头文件（.h）和源文件（.c）来组织代码。
*   **终端高级控制**：使用 `termios.h` 实现非阻塞输入，以及使用ANSI转义序列实现无闪烁的界面刷新。
*   **游戏逻辑实现**：将游戏规则（如碰撞、旋转、消行）转化为具体的算法和代码。
*   **编译与构建**：使用 `Makefile` 来自动化编译过程。

## 2. 项目结构
建议采用以下文件结构，这有助于保持代码的整洁和模块化：

```
tetris/
├── Makefile        # 编译脚本
├── tetris.h        # 头文件，用于存放声明和定义
└── main.c          # 源文件，用于存放功能的具体实现
```

## 3. 核心数据结构 (定义在 `tetris.h`)

*   **游戏区域 `playfield`**：一个 `int[ROWS][COLS]` 的二维数组。`0` 代表空格，非 `0` 代表有方块。
*   **方块 `Tetromino`**：一个结构体，包含方块的4x4形状数组、在游戏区域的坐标`(x, y)`和尺寸。
*   **所有方块形状 `tetromino_shapes`**：一个全局的、只读的多维数组，存放所有7种方块的各种旋转形态。这是游戏的数据核心。

## 4. 关键功能模块实现指南
你需要逐一实现以下函数。每个函数都是游戏的一个独立构建块。

### a. 终端控制 (`main.c`)

*   `init_termios()` / `reset_termios()`：这部分代码已经为你提供。它们负责设置终端，使其能立即响应单个按键而无需等待回车。**理解其作用即可**：在程序开始时调用 `init`，在结束时务必调用 `reset`。
*   `getch_nonblock()`：同样已为你提供。它会尝试读取一个字符，如果没有按键则立即返回`0`，不会阻塞程序。

### b. 游戏主流程 (`main.c`)

*   `main()` 函数：
    *   **初始化**：调用 `init_termios()`，用 `srand(time(NULL))` 初始化随机数种子，调用 `init_game()` 设置游戏初始状态。
    *   **主循环 `while (!game_over)`**：
        *   计算时间差，以实现稳定的自动下落。
        *   调用 `handle_input()` 处理玩家操作。
        *   根据时间判断是否需要执行自动下落。
        *   调用 `draw_game()` 刷新屏幕。
        *   短暂休眠 (`usleep`) 防止CPU占用100%。
    *   **清理**：循环结束后，显示“Game Over”信息，并调用 `reset_termios()` 恢复终端。

### c. 核心逻辑 (需要你重点实现)

*   `generate_new_piece()`：
    *   **目标**：创建一个新的方块并置于游戏区域顶部中央。
    *   **步骤**：
        1.  随机选择一个方块类型（0-6）。
        2.  从 `tetromino_shapes` 数组中复制对应形状到 `current_piece.shape`。
        3.  设置 `current_piece.size` (例如，I是4x4，O是2x2，其他是3x3)。
        4.  设置初始坐标 `current_piece.x` 为场地中央，`current_piece.y` 为`0`（或负数，使其从屏幕外进入）。

*   `check_collision(Tetromino piece)`：
    *   **目标**：判断给定的 `piece` 是否处于一个非法位置。这是**最重要**的函数。
    *   **步骤**：
        1.  遍历 `piece.shape` 4x4矩阵。
        2.  对于每个非零格，计算其在 `playfield` 中的绝对坐标 `(board_x, board_y)`。
        3.  检查三个条件，**任何一个为真都意味着碰撞**：
            *   `board_x < 0` 或 `board_x >= COLS` (超出左右边界)。
            *   `board_y >= ROWS` (超出下边界)。
            *   `board_y >= 0` 并且 `playfield[board_y][board_x] != 0` (与已固定的方块重叠)。
        4.  如果所有非零格都合法，返回 `false` (无碰撞)。

*   `handle_input()`：
    *   **目标**：响应玩家的 `'a'`, `'d'`, `'s'`, `'w'` 等按键。
    *   **步骤**：
        1.  调用 `getch_nonblock()` 获取按键。
        2.  如果没有按键 (`== 0`)，直接返回。
        3.  创建一个 `current_piece` 的临时副本 `temp_piece`。
        4.  根据按键修改 `temp_piece` 的 `x`, `y` 或调用 `rotate_piece(&temp_piece)`。
        5.  调用 `check_collision(temp_piece)`。
        6.  如果**不碰撞**，则将 `temp_piece` 的状态更新回 `current_piece`。

*   `rotate_piece(Tetromino *piece)`：
    *   **目标**：将 `piece` 顺时针旋转90度。
    *   **步骤 (矩阵旋转算法)**：
        1.  创建一个临时的4x4数组 `temp_shape`。
        2.  **转置**：`temp_shape[c][r] = piece->shape[r][c]`。
        3.  **行翻转**：将 `temp_shape` 的每一行进行翻转。
        4.  将 `temp_shape` 的内容复制回 `piece->shape`。

*   `lock_piece()`：
    *   **目标**：当方块无法再下落时，将其“固化”到游戏区域中。
    *   **步骤**：
        1.  遍历 `current_piece.shape`。
        2.  对于每个非零格，将其值（比如1）写入 `playfield` 对应的绝对坐标中。

*   `clear_lines()`：
    *   **目标**：检查并消除所有填满的行。
    *   **步骤**：
        1.  从 `ROWS - 1` (最底行) 向上遍历到 `0`。
        2.  对每一行，检查是否所有列都非零。
        3.  如果是满行：
            *   记录消行数（用于计分）。
            *   将该行之上的所有行整体向下移动一行（用 `memmove` 或循环实现）。
            *   将最顶行（`playfield[0]`）清零。
            *   因为行下移了，所以下一轮循环需要**再次检查当前行** (通过 `r++` 或 `continue` 实现)。

*   `draw_game()`：
    *   **目标**：在终端上绘制游戏画面。
    *   **步骤**：
        1.  使用 `printf("\x1b[H\x1b[2J")` 清屏并归位光标。
        2.  创建一个临时的二维数组 `display_buffer`，大小与 `playfield` 相同。
        3.  先将 `playfield` 的内容复制到 `display_buffer`。
        4.  再将 `current_piece` 的形状“画”在 `display_buffer` 上。
        5.  遍历 `display_buffer`，打印边框和内容（`0` 打印空格，非`0` 打印 `[]`）。
        6.  打印分数等其他信息。
        7.  调用 `fflush(stdout)` 确保立即显示。

## 5. 编译与运行

1.  将提供的三个代码块分别保存为 `Makefile`, `tetris.h`, `main.c`。
2.  在WSL终端中，进入 `tetris` 目录。
3.  执行 `make` 命令。如果一切顺利，会生成一个名为 `tetris` 的可执行文件。
4.  执行 `./tetris` 来开始游戏。

**你的任务就是将 `main.c` 中标记为 `// TODO:` 的函数体填充完整！**