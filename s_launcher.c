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

	for (int t = 3; t > 0; t--) {
		char msg[32];
		snprintf(msg, 32, "Starting in %d...\n", t);
		for (int i = 0; i < num_players; i++) {
			send(players[i].socket, msg, strlen(msg), 0);
		}
		sleep(1);
	}

	if (num_players == 2) {
		char start[] = "START\n";
		send(players[0].socket, start, strlen(start), 0);
		send(players[1].socket, start, strlen(start), 0);

		if (fork() == 0) {
			close(listen_socket);
			dup2(players[0].socket, 3);
			dup2(players[1].socket, 4);
			execl("./server", "./server", "3", "4", NULL);
			exit(1);
		}
		wait(NULL);
		close(listen_socket);

	} else if (num_players == 4) {
		int winners[2];
		int status;


		printf("Starting Semifinal A...\n");
		send(players[0].socket, "START\n", 6, 0);
		send(players[1].socket, "START\n", 6, 0);
		send(players[2].socket, "Waiting for Semifinal A to finish...\n", 37, 0);
		send(players[3].socket, "Waiting for Semifinal A to finish...\n", 37, 0);

		if (fork() == 0) {
			close(listen_socket);
			close(players[2].socket);
			close(players[3].socket);
			dup2(players[0].socket, 3);
			dup2(players[1].socket, 4);
			execl("./server", "./server", "3", "4", NULL);
			exit(1);
		}
		wait(&status);
		winners[0] = WEXITSTATUS(status); // 0 or 1


		int loser1 = 1 - winners[0];
		send(players[loser1].socket, "You lost. Game Over.\n", 21, 0);
		close(players[loser1].socket);

		// --- MATCH 2: Semifinal B (Player 2 vs Player 3) ---
		printf("Starting Semifinal B...\n");
		send(players[2].socket, "START\n", 6, 0);
		send(players[3].socket, "START\n", 6, 0);
		send(players[winners[0]].socket, "You won! Waiting for Semifinal B...\n", 36, 0);

		if (fork() == 0) {
			close(listen_socket);
			close(players[winners[0]].socket);
			dup2(players[2].socket, 3);
			dup2(players[3].socket, 4);
			execl("./server", "./server", "3", "4", NULL);
			exit(1);
		}
		wait(&status);

		winners[1] = 2 + WEXITSTATUS(status);

		int loser2 = (winners[1] == 2) ? 3 : 2;
		send(players[loser2].socket, "You lost. Game Over.\n", 21, 0);
		close(players[loser2].socket);

		printf("Starting Grand Finals: Player %s vs Player %s\n", 
				players[winners[0]].name, players[winners[1]].name);

		// Final "START" signal for the finalists
		send(players[winners[0]].socket, "START\n", 6, 0);
		send(players[winners[1]].socket, "START\n", 6, 0);

		if (fork() == 0) {
			close(listen_socket);
			dup2(players[winners[0]].socket, 3);
			dup2(players[winners[1]].socket, 4);
			execl("./server", "./server", "3", "4", NULL);
			exit(1);
		}
		wait(&status);

		int champion_idx = (WEXITSTATUS(status) == 1) ? winners[0] : winners[1];
		printf("Tournament Over! Winner: %s\n", players[champion_idx].name);

		close(players[winners[0]].socket);
		close(players[winners[1]].socket);
		close(listen_socket);
	}
	return 0;
}
