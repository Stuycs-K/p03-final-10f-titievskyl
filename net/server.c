#include "networking.h"
/*
 *  process 
 *  Who needs JSON?
 *  parse player data string 'packets' into relevant numbers: ID, state, HP, x/y
 *  maybe orientation in the future ... 
 */

void process(char * buff){
	return 0;
}
void subserver_logic(int client_socket){
	char inbuf[BUFFER_SIZE];
	int gamebuff[8][8] = {0};
	while (1) {
		memset(inbuf, 0, sizeof(inbuf));
		printf("Awaiting data...\n");
		int n = recv(client_socket, inbuf, BUFFER_SIZE, MSG_WAITALL);
		if (n <= 0){ 
			printf("Subserver killed!\n");
			break;
		}
		printf("Recieved! processing... \n");
		rotX(inbuf, 13);
		send(client_socket, inbuf, BUFFER_SIZE, 0);
		printf("Sent!\n");
	}

	close(client_socket);
}

int main(int argc, char *argv[] ) { 
	int listen_socket = server_setup();
	printf("setup complete || listen_socket: %d \n", listen_socket);

	while (1) {
		int client_socket = server_tcp_handshake(listen_socket);

		if (fork() == 0) {
			close(listen_socket);
			subserver_logic(client_socket);
			exit(0);
		}

		close(client_socket);
	}
}
