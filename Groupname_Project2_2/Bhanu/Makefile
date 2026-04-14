CC     = gcc
CFLAGS = -Wall -Wextra -std=c99
SRC    = src/main.c src/mlfq.c src/edf.c
TARGET = scheduler_sim

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
