CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
TARGET = main
SRC = main.c dijkstra.c

all: $(TARGET)

$(TARGET): $(SRC) dijkstra.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) *.o

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
