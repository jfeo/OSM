.PHONY: clean run

CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -pthread -fdiagnostics-color=always
SOURCES=queue.c test.c
EXECUTABLE=queuetest

default: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES)
	./$(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -rf $(EXECUTABLE)
