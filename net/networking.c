#include "networking.h"

/*Create and bind a socket.
 * Place the socket in a listening state.
 * returns the socket descriptor
 */
int server_setup() {
	//setup structs for getaddrinfo
	struct addrinfo hints, *res;  
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	//create the socket
	int clientd;//store the socket descriptor here
	getaddrinfo(NULL, PORT, &hints, &res);
	clientd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	//printf("Client socket created: %d", clientd);
	//this code should get around the address in use error
	int yes = 1;
	int sockOpt =  setsockopt(clientd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	err(sockOpt,"sockopt  error");

	//bind the socket to address and port
	bind(clientd, res->ai_addr, res->ai_addrlen);
	//set socket to listen state
	listen(clientd, 10);

	//free the structs used by getaddrinfo
	freeaddrinfo(res);
	//printf("SETUP GOOD!\n");
	return clientd;
}

/*Accept a connection from a client
 *return the socket descriptor for the new socket connected to the client
 *blocks until connection is made.
 */
int server_tcp_handshake(int listen_socket){
	int client_socket;
	socklen_t sock_size;
	struct sockaddr_in client_address;
	sock_size = sizeof(client_address);
	client_socket = accept(listen_socket,(struct sockaddr *)&client_address, &sock_size);

	//accept() the client connection
	//printf("servertcp done\n");
	return client_socket;
}


/*Connect to the server
 *return the to_server socket descriptor
 *blocks until connection is made.*/
int client_tcp_handshake(char * server_address) {
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;  // Either IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;
	//getaddrinfo
	getaddrinfo(server_address, PORT, &hints, &res);	
	int serverd;//store the socket descriptor here
		    //create the socket
	serverd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	err(connect(serverd, res->ai_addr, res->ai_addrlen), "connect");
		    //connect() to the server
	//printf("SERVERD: %d", serverd);
	freeaddrinfo(res);

	return serverd;
}




void err(int i, char*message){
	if(i < 0){
		printf("Error: %s - %s\n",message, strerror(errno));
		exit(1);
	}
}
