CC := gcc -g -std=c99
SOURCES := queue.c test.c
TARGET  := queue

.PHONY: all clean

all: clean $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) -o $(TARGET) $(SOURCES)

