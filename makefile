
CC = gcc
CFLAGS = -Wall -g -I/usr/include/SDL2 -I SDL_ttf
TARGET = prog
SOURCES = client.c util/ray.c util/main.h net/networking.h net/networking.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o  $@ $^ -lm -lSDL2 -lSDL2main -lSDL2_ttf 

clean:
	rm -f $(TARGET) *.o
	rm -f server
server: net/networking.c net/networking.h net/server.c
	$(CC) -Wall -o server  net/server.c net/networking.c net/networking.h
