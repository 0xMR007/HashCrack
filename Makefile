CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wno-deprecated-declarations -Iinclude -std=c99 -Wpedantic -Wconversion -g
LDFLAGS=-lcrypto
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
TARGET=hashcrack

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) src/*.o