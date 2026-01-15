#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    char mode[10];
    printf("1, 2, or 4 players? ");
    fgets(mode, sizeof(mode), stdin);
    
    if (mode[0] == '2') {
        execvp("./server", (char *[]){"./server", NULL});
    } else if (mode[0] == '1') {
        printf("Single player not implemented\n");
    } else if (mode[0] == '4') {
        printf("Four player not implemented\n");
    }
    
    return 0;
}