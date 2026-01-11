
CC = gcc
CFLAGS = -Wall -g `sdl2-config --cflags --libs`
TARGET = prog
SOURCES = client.c util/ray.c util/main.h net/networking.h net/networking.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o  $@ $^ -lm -lSDL2 -lSDL2main 

clean:
	rm -f $(TARGET) *.o
	rm -f server
server: net/networking.c net/networking.h net/server.c
	$(CC) -Wall -o server  net/server.c net/networking.c net/networking.h