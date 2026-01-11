#include "networking.h"
/*
 *  process 
 *  Who needs JSON?
 *  parse player data string 'packets' into relevant numbers: ID, state, HP, x/y
 *  maybe orientation in the future ... 
 */

struct PlayerState {
	int  ID; //ids are unique (will handshake integrate). going to use these as index hash for o(1) state sorting in a list of players
		 //"moves" will be processed by ID order, if PID%1024, should be pretty random/fair
	int  State;
	int  HP;
	float  x;
	float  y;
	//struct PlayerState * previous --- if i need to trace i'll make this a linked list
};
struct PlayerState PLAYERS[2];
int player_count = 0;

void process(char *buff, struct PlayerState *p) {
    sscanf(buff, "%d %d %d %f %f", &(p->ID), &(p->State), &(p->HP), &(p->x), &(p->y));
}

void subserver_logic(int client_socket, int player_id) {
    char inbuf[BUFFER_SIZE];
    
    while (1) {
        memset(inbuf, 0, sizeof(inbuf));
        int n = recv(client_socket, inbuf, BUFFER_SIZE, MSG_WAITALL);
        if (n <= 0) {
            printf("Player %d disconnected\n", player_id);
            break;
        }
        
        process(inbuf, &PLAYERS[player_id]);
        printf("Player %d: State=%d HP=%d x=%.2f y=%.2f\n", 
               PLAYERS[player_id].ID, PLAYERS[player_id].State, 
               PLAYERS[player_id].HP, PLAYERS[player_id].x, PLAYERS[player_id].y);
    }
    
    close(client_socket);
}

int main(int argc, char *argv[]) { 
    int listen_socket = server_setup();
    printf("Server ready for 2 players\n");

    while (player_count < 2) {
        int client_socket = server_tcp_handshake(listen_socket);
        
        if (fork() == 0) {
            close(listen_socket);
            subserver_logic(client_socket, player_count);
            exit(0);
        }
        
        printf("Player %d connected\n", player_count);
        player_count++;
        close(client_socket);
    }
    
    wait(NULL);
    wait(NULL);
    close(listen_socket);
}