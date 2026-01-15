#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    char ip[100];
    printf("Server IP: ");
    fgets(ip, sizeof(ip), stdin);
    ip[strcspn(ip, "\n")] = 0;
    
    execvp("./client", (char *[]){"./client", ip, NULL});
    
    return 0;
}