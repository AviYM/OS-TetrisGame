/*
Avi Miletzky
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#define ERROR -1
#define S_C_ERROR "Error in system call"
#define BOARD_SIZE 20

enum State {VERTICAL, HORIZONTAL};
//enum TypeShape{I, J, L, O, S, T, Z};

typedef struct Point{
    int x;
    int y;
}Point;

typedef struct Shape{
    Point position;
    enum State state;
    //enum TypeShape typeShape;
}Shape;

Shape shape;

void errorsManager(char *error) {
    write(STDERR_FILENO, error, strlen(error));
    exit(0);
}

void moveDown() {
    if (shape.position.y < BOARD_SIZE - 2) {
        ++(shape.position.y);
    } else if ((shape.state == HORIZONTAL)
        && (shape.position.y < BOARD_SIZE - 1)) {
        ++(shape.position.y);
    }
}

void moveRight() {
    if (shape.position.x < BOARD_SIZE - 3) {
        ++(shape.position.x);
    } else if ((shape.state == VERTICAL)
        && (shape.position.x < BOARD_SIZE - 2)) {
        ++(shape.position.x);
    }
}

void moveLeft() {
    if (shape.position.x > 2) {
        --(shape.position.x);
    } else if ((shape.state == VERTICAL) && (shape.position.x > 1)) {
        --(shape.position.x);
    }
}

void changeShapeState() {
    if (shape.position.y == 0) {
        ++(shape.position.y);
    }
    if (shape.position.x != 1 && shape.position.x != BOARD_SIZE - 2
        && shape.position.y < BOARD_SIZE - 2) {
        shape.state = (shape.state == VERTICAL)? HORIZONTAL: VERTICAL;
    }
}

void initBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    int i, j;

    for (i = 0; i < BOARD_SIZE; ++i) {
        for (j = 0; j < BOARD_SIZE; ++j) {
            if (j == 0 || j == BOARD_SIZE - 1 || i == BOARD_SIZE - 1) {
                board[i][j] = '*';
            } else {
                board[i][j] = ' ';
            }
        }
    }
}

void placesShapeOnTheBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    if (shape.state == VERTICAL) {
        board[shape.position.y - 1][shape.position.x] = '-';
        board[shape.position.y][shape.position.x] = '-';
        board[shape.position.y + 1][shape.position.x] = '-';
    } else {
        board[shape.position.y][shape.position.x - 1] = '-';
        board[shape.position.y][shape.position.x] = '-';
        board[shape.position.y][shape.position.x + 1] = '-';
    }
}

void printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    int i;
    char newLine = '\n';

    for (i = 0; i < BOARD_SIZE; ++i) {
        if (write(STDOUT_FILENO, &board[i], BOARD_SIZE) == ERROR
            || write(STDOUT_FILENO, &newLine, 1) == ERROR) {
            errorsManager(S_C_ERROR);
        }
    }
}

/*void getRandomTypeShape() {
    shape.typeShape = rand() % 7;
}*/

void displaysBoard() {
    char board[BOARD_SIZE][BOARD_SIZE];
    int i, j;

    if (system("clear") == ERROR) {
        errorsManager(S_C_ERROR);
    }

    initBoard(board);
    placesShapeOnTheBoard(board);
    printBoard(board);

    if ((shape.state == VERTICAL) && (shape.position.y >= BOARD_SIZE - 2)) {
        shape.position.y = 0;
        shape.position.x = BOARD_SIZE / 2;
        shape.state = HORIZONTAL;
        displaysBoard();
    } else if ((shape.state == HORIZONTAL)
        && (shape.position.y >= BOARD_SIZE - 1)) {
        shape.position.y = 0;
        shape.position.x = BOARD_SIZE / 2;
        displaysBoard();
    }
}

void shapeNavigator()
{
    char newCommand;
    //read char-command from the pipe and do it.
    if (read(STDIN_FILENO, &newCommand, 1) == ERROR) {
        errorsManager(S_C_ERROR);
    }
    switch(newCommand) {
        case 'a': moveLeft();
            break;
        case 'd': moveRight();
            break;
        case 's': moveDown();
            break;
        case 'w': changeShapeState();
            break;
        case 'q':
            exit(0);
    }
    displaysBoard();
    signal(SIGUSR2, shapeNavigator);
}

void alarmCallBack() {
    alarm(1);
    signal(SIGALRM, alarmCallBack);
    moveDown();
    displaysBoard();
}

int main() {
    shape.position.x = BOARD_SIZE / 2;
    shape.position.y = 0;
    shape.state = HORIZONTAL;

    signal(SIGALRM, alarmCallBack);
    alarm(1);
    signal(SIGUSR2, shapeNavigator);

    while (1) {
        pause();
    }
}