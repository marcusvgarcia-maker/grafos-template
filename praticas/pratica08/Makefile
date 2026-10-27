CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
TARGET = main
SRC = main.c agm.c

ifeq ($(OS),Windows_NT)
TARGET = main.exe
endif

all: $(TARGET)

$(TARGET): $(SRC) agm.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

test: run

clean:
	rm -f main main.exe *.o

.PHONY: all run test clean
