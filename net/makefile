CC=gcc
CFLAGS=-Wall -g
compile: client.c server.c networking.c
	@$(CC) $(CFLAGS) -o client client.c networking.c
	@$(CC) $(CFLAGS) -o server server.c networking.c
clean:
	@rm -f *.o client server

