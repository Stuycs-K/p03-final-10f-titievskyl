#include "networking.h"
#include <string.h>

struct PlayerState {
	int  ID;
	int  State;
	int  HP;
	float  x;
	float  y;
	float rot;
};

struct PlayerState PLAYERS[2];
int client_sockets[2] = {-1, -1};

int process(char *buff, struct PlayerState *p) {
    int parsed = sscanf(buff, "%d %d %d %f %f %f", &(p->ID), &(p->State), &(p->HP), &(p->x), &(p->y), &(p->rot));
    return (parsed == 6); // checks
}
int main(int argc, char *argv[]) { 
	int listen_socket;// = server_setup();

	if(argc > 2){
		client_sockets[0] = atoi(argv[1]);
		client_sockets[1] = atoi(argv[2]);
	}else{
		listen_socket = server_setup();
		printf("Listen socket: %d\n", listen_socket);
		printf("Waiting for 2 players...\n");
		for (int i = 0; i < 2; i++) {
			printf("Waiting for player %d...\n", i);
			client_sockets[i] = server_tcp_handshake(listen_socket);
			printf("Player %d connected on socket %d\n", i, client_sockets[i]);
		}
	}


	printf("Both players connected");

	fd_set read_fds;
	char inbuf[BUFFER_SIZE];

	while (1) {
		FD_ZERO(&read_fds);
		int max_fd = -1;

		for (int i = 0; i < 2; i++) {
			if (client_sockets[i] != -1) {
				FD_SET(client_sockets[i], &read_fds);
				if (client_sockets[i] > max_fd) {
					max_fd = client_sockets[i];
				}
			}
		}

		struct timeval timeout = {1, 0};
		int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);

		if (activity < 0) {
			printf("Select error\n");
			break;
		}

		if (activity > 0) {
			for (int i = 0; i < 2; i++) {
				if (client_sockets[i] != -1 && FD_ISSET(client_sockets[i], &read_fds)) {
					memset(inbuf, 0, sizeof(inbuf));
					int n = recv(client_sockets[i], inbuf, BUFFER_SIZE - 1, 0);
					printf("Recv from player %d: %d bytes\n", i, n);
					if (n <= 0) {
						printf("Player %d disconnected\n", i);
						close(client_sockets[i]);
						client_sockets[i] = -1;
					} else {
						inbuf[n] = '\0';
						if(process(inbuf, &PLAYERS[i])){
						printf("Player %d: ID=%d State=%d HP=%d x=%.2f y=%.2f\n rot=%.2f", 
								i, PLAYERS[i].ID, PLAYERS[i].State, 
								PLAYERS[i].HP, PLAYERS[i].x, PLAYERS[i].y, PLAYERS[i].rot);
						if(PLAYERS[i].State == 3){
							//	PLAYERS[i+1%2].x = 1000000; //to narnia..
						}
						for (int j = 0; j < 2; j++) {
							if (j != i && client_sockets[j] != -1) {
								n = send(client_sockets[j], inbuf, n, 0);
								printf("sent to player %d: %d bytes\n", i, n);
							}
						}
					}
					}
				}
			}
		}
		for (int i = 0; i < 2; i++) {
			if (PLAYERS[i].HP <= 0 &&PLAYERS[i].ID!=0) {
				exit(1 - i); 
			}
		}
		usleep((int)(1/60.0f * 100000));
	}

	for (int i = 0; i < 2; i++) {
		if (client_sockets[i] != -1) close(client_sockets[i]);
	}
	close(listen_socket);
	return 0;
}
