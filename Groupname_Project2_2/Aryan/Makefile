CC = gcc
CFLAGS = -Wall -Iinclude
SRC = $(wildcard src/*.c)
TARGET = scheduler_sim


all: $(TARGET)

$(TARGET): $(SRC)
		$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
		rm -f $(TARGET)