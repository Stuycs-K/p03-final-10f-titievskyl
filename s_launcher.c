#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "net/networking.h"
#include <string.h>
#include <sys/wait.h>

struct Player {
	int socket;
	char name[32];
	int ready;
};

int main(int argc, char *argv[]) {
	int num_players = 2;
	if (argc > 1) num_players = atoi(argv[1]);

	struct Player players[4] = {0};
	int listen_socket = server_setup();

	for (int i = 0; i < num_players; i++) {
		players[i].socket = server_tcp_handshake(listen_socket);
	}

	for (int i = 0; i < num_players; i++) {
		recv(players[i].socket, players[i].name, 31, 0);
		players[i].ready = 1;

		char roster[256] = "Players:\n";
		for (int j = 0; j <= i; j++) {
			char line[64];
			snprintf(line, 64, "%s\n", players[j].name);
			strcat(roster, line);
		}
		for (int j = 0; j <= i; j++) {
			send(players[j].socket, roster, strlen(roster), 0);
		}
	}

	for (int t = 10; t > 0; t--) {
		char msg[32];
		snprintf(msg, 32, "Starting in %d...\n", t);
		for (int i = 0; i < num_players; i++) {
			send(players[i].socket, msg, strlen(msg), 0);
		}
		sleep(1);
	}

	char start[] = "START\n";
	for (int i = 0; i < num_players; i++) {
		send(players[i].socket, start, strlen(start), 0);
	}

	if (num_players == 2) {
		if (fork() == 0) {
			close(listen_socket);
			dup2(players[0].socket, 3);
			dup2(players[1].socket, 4);
			execl("./server", "./server", "3", "4", NULL);
			exit(1);
		}
		close(players[0].socket);
		close(players[1].socket);
		close(listen_socket);
		wait(NULL);
	}

	return 0;
}
