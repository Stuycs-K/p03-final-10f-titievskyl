
CC = gcc
CFLAGS = -Wall -g `sdl2-config --cflags --libs`
TARGET = prog
SOURCES = client.c  util/ray.c util/main.h

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o  $@ $^ -lm -lSDL2 -lSDL2main 

clean:
	rm -f $(TARGET) *.o