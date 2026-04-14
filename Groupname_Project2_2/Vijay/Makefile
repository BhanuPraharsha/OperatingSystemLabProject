# Makefile for CPU Scheduling Simulation
# Simple build system for Windows using gcc

CC = gcc
CFLAGS = -Wall -g
TARGET = scheduler.exe
SOURCES = main.c algorithms.c
HEADERS = algorithms.h

# Build the executable
all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# Build and run the simulation
run: $(TARGET)
	./$(TARGET)

# Clean up generated files
clean:
	del /Q $(TARGET) 2>nul || true

.PHONY: all run clean
