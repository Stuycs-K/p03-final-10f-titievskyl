
CC = gcc
CFLAGS =  -g -I/usr/include/SDL2 -I SDL_ttf
TARGET = prog
SOURCES = client.c util/ray.c util/main.h net/networking.h net/networking.c

all: $(TARGET)

prog: client.c util/ray.c util/main.h net/networking.h net/networking.c
	@$(CC) $(CFLAGS) -o  $@ $^ -lm -lSDL2 -lSDL2main -lSDL2_ttf 

launcher:
	@$(CC) -Wall -o client_launcher c_launcher.c net/networking.c net/networking.h
clean:
	rm -f $(TARGET) *.o
	rm -f server
	rm -f server_launcher

server: net/networking.c net/networking.h net/server.c
	@$(CC) -Wall -o server_launcher s_launcher.c net/networking.c net/networking.h
	@$(CC) -Wall -o server  net/server.c net/networking.c net/networking.h

build: prog server
	@echo "built!"
	
	
