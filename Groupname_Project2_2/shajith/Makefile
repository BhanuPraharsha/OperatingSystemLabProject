CC = gcc
CFLAGS = -Wall -std=c99
SRC = src/main.c src/lottery.c src/mlfq.c
TARGET = scheduler_sim

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
