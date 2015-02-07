.PHONY: clean run

CC=gcc
CFLAGS=-Wall -pedantic -std=c11
SOURCES=heap.c test.c
EXECUTABLE=heaptest

default: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -rf $(EXECUTABLE)
