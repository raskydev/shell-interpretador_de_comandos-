CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c11
TARGET  = rush
SRC     = rush.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

