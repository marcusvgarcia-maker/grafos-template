CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
TARGET = main
SRC = main.c bellman_ford.c

all: $(TARGET)

$(TARGET): $(SRC) bellman_ford.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: all run clean
