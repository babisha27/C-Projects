CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99

SRC     = $(wildcard *.c)
OBJ     = $(SRC:.c=.o)
TARGET  = lms

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c lms.h
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all run clean
