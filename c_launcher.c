#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "net/networking.h"

void wait_for_game_start() {
    char name[32];
    printf("Enter name: ");
    fgets(name, 32, stdin);
    name[strcspn(name, "\n")] = 0;
    send(server_socket, name, strlen(name), 0);
    
    char buf[256];
    while (1) {
        int n = recv(server_socket, buf, 255, 0);
        if (n > 0) {
            buf[n] = 0;
            printf("%s", buf);
            fflush(stdout);
            if (strstr(buf, "START")) break;
        }
    }
}