# General compilation settings
CC = gcc
CFLAGS = -Wall -g -Isrc/client
LDFLAGS = -lssl -lcrypto

# Define paths for the source, object, and binary outputs
SRC_DIR = src/client
OBJ_DIR = build/obj/client
BIN_DIR = build/bin
EXEC = $(BIN_DIR)/client_app

# Source and object file associations
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Default target
all: $(EXEC)

# Linking the executable from the object files
$(EXEC): $(OBJECTS) | $(BIN_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compiling every C file to object file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

# Ensure the directories exist
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Clean up
clean:
	rm -rf build/obj build/bin

.PHONY: all clean
