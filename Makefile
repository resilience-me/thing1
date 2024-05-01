# Makefile for SSL Client

CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lssl -lcrypto
OBJ=main.o network.o commands.o
EXEC=ssl_client

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: clean
