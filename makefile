CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99
LDLIBS  = -lm

SRC     = $(wildcard *.c)
OBJ     = $(SRC:.c=.o)
TARGET  = student

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDLIBS)

%.o: %.c student.h
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all run clean
