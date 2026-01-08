#include "networking.h"
#include <signal.h>

int server_socket = -1;

void cleanup(int signo) {
	if (server_socket != -1) {
		close(server_socket);
	}
	exit(0);
}

void clientLogic(int server_socket){
	char buff[BUFFER_SIZE];
	char inbuf[BUFFER_SIZE];

	printf("\nENTER A STRING:\n");
	memset(buff, 0, sizeof(buff));
	if(fgets(buff, sizeof(buff), stdin)==NULL){
		exit(0);
	}
	buff[strcspn(buff, "\n")] = '\0';
	send(server_socket, buff, BUFFER_SIZE, 0);
	if(recv(server_socket, inbuf, BUFFER_SIZE, MSG_WAITALL)==1){
		exit(0);
	}

	printf("results from server: %s", inbuf);
	if(strlen(inbuf)==0){
		exit(0);
	}
}

int main(int argc, char *argv[] ) {
	signal(SIGINT, cleanup);
	char* IP = "127.0.0.1";
	if(argc>1){
		IP=argv[1];
	}
	server_socket = client_tcp_handshake(IP);
	printf("client connected || Socket Descriptor: %d\n", server_socket);
	while(1){
		clientLogic(server_socket);
	}
}
