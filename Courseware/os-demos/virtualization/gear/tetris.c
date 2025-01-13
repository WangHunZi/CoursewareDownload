// Download toybox.h:
// https://git.nju.edu.cn/jyy/toybox

#include <toybox.h>
#include <stdlib.h>
#include <time.h>

#define BOARD_WIDTH  10
#define BOARD_HEIGHT 20
#define BLOCK_SIZE   4

int board[BOARD_HEIGHT][BOARD_WIDTH] = {0};
int block[BLOCK_SIZE][BLOCK_SIZE] = {0};
int blockX, blockY;

void generateBlock() {
    blockX = BOARD_WIDTH / 2 - BLOCK_SIZE / 2;
    blockY = 0;
    
    for (int i = 0; i < BLOCK_SIZE; i++)
        for (int j = 0; j < BLOCK_SIZE; j++)
            block[i][j] = 0;

    int blockType = rand() % 7;
    switch (blockType) {
        case 0: // I
            block[1][0] = block[1][1] = block[1][2] = block[1][3] = 1;
            break;
        case 1: // J
            block[0][1] = block[1][1] = block[2][0] = block[2][1] = 1; 
            break;
        case 2: // L
            block[0][0] = block[1][0] = block[2][0] = block[2][1] = 1;
            break;
        case 3: // O
            block[0][0] = block[0][1] = block[1][0] = block[1][1] = 1;
            break;
        case 4: // S
            block[1][0] = block[1][1] = block[0][1] = block[0][2] = 1;
            break;
        case 5: // T
            block[0][0] = block[0][1] = block[0][2] = block[1][1] = 1;
            break;
        case 6: // Z
            block[0][0] = block[0][1] = block[1][1] = block[1][2] = 1;
            break;
    }
}

int isValid(int x, int y, int block[BLOCK_SIZE][BLOCK_SIZE]) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            if (block[i][j]) {
                int newX = x + j;
                int newY = y + i;
                if (newX < 0 || newX >= BOARD_WIDTH || newY < 0 || newY >= BOARD_HEIGHT || board[newY][newX]) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

void rotateBlock() {
    int temp[BLOCK_SIZE][BLOCK_SIZE] = {0};
    for (int i = 0; i < BLOCK_SIZE; i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            temp[i][j] = block[BLOCK_SIZE - 1 - j][i];
        }
    }

    if (isValid(blockX, blockY, temp)) {
        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                block[i][j] = temp[i][j];
            }
        }
    }
}

void mergeBlock() {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            if (block[i][j]) {
                board[blockY + i][blockX + j] = 1;
            }
        }
    }
}

void clearLines() {
    for (int i = BOARD_HEIGHT - 1; i >= 0; i--) {
        int isFull = 1;
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (!board[i][j]) {
                isFull = 0;
                break;
            }
        }
        
        if (isFull) {
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < BOARD_WIDTH; j++) {
                    board[k][j] = board[k - 1][j];
                }
            }
            i++;
        }
    }
}

void drawBoard(draw_function draw) {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (board[i][j]) {
                draw(j, i, '#');
            } else {
                draw(j, i, '.');
            }
        }
    }
    
    for (int i = 0; i < BLOCK_SIZE; i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            if (block[i][j]) {
                draw(blockX + j, blockY + i, '@');
            }
        }
    }
}

void update(int w, int h, draw_function draw) {
    if (!isValid(blockX, blockY + 1, block)) {
        mergeBlock();
        clearLines();
        generateBlock();
        
        if (!isValid(blockX, blockY, block)) {
            // Game Over
            draw(3, 10, 'G');
            draw(4, 10, 'A');
            draw(5, 10, 'M');
            draw(6, 10, 'E');
            draw(8, 10, 'O'); 
            draw(9, 10, 'V');
            draw(10, 10, 'E');
            draw(11, 10, 'R');
            return;
        }
    } else {
        blockY++;
    }
    
    drawBoard(draw);
}

void keypress(int key) {
    switch (key) {
        case 'a':
            if (isValid(blockX - 1, blockY, block)) {
                blockX--;
            }
            break;
        case 'd':
            if (isValid(blockX + 1, blockY, block)) {
                blockX++;
            }
            break;
        case 'w':
            rotateBlock();
            break;
        case 's':
            if (isValid(blockX, blockY + 1, block)) {
                blockY++;
            }
            break;
    }
}

int main() {
    srand(time(0));
    generateBlock();
    toybox_run(5, update, keypress);
}
