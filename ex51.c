/*
Avi Miletzky
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <termios.h>
#include <stdbool.h>

#define ERROR -1
#define S_C_ERROR "Error in system call"
#define RUN_PROG "./draw.out"

void errorsManager(char *error) {
    write(STDERR_FILENO, error, strlen(error));
}

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return (buf);
}

bool isValidChar(char input) {
    if (input == 'a' || input == 's' || input == 'd' || input == 'w'
        || input == 'q') {
        return true;
    }
    return false;
}

int main() {
    pid_t pidChild;
    int pipeD[2];
    char inChar, *execFileArgs[2] = {RUN_PROG, NULL};

    if (pipe(pipeD) == ERROR) {
        errorsManager(S_C_ERROR);
        return ERROR;
    }

    pidChild = fork();
    if (pidChild == ERROR) {
        errorsManager(S_C_ERROR);
        return ERROR;
    }
    if (pidChild == 0) {
        dup2(pipeD[0], STDIN_FILENO);
        execvp(execFileArgs[0], execFileArgs);
        errorsManager(S_C_ERROR);
        return ERROR;
    }

    while (true) {
        if (isValidChar((inChar = getch()))) {
            if (write(pipeD[1], &inChar, sizeof(char)) == ERROR) {
                errorsManager(S_C_ERROR);
                return ERROR;
            }
            if (kill(pidChild, SIGUSR2) == ERROR) {
                errorsManager(S_C_ERROR);
                return ERROR;
            }
            if (inChar == 'q') {
                break;
            }
        }
    }
}