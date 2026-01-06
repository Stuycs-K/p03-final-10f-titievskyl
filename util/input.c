#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <math.h>
#include "main.h"

static struct termios termios_orig;
void raw_begin(void) {
    tcgetattr(STDIN_FILENO, &termios_orig);
    struct termios raw = termios_orig;
    raw.c_lflag &= ~(ICANON | ECHO | ISIG);
    raw.c_cc[VMIN] = 1;  
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void raw_end(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &termios_orig);
}

