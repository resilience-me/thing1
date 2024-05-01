# General compilation settings
CC = gcc
CFLAGS = -Wall -g -Isrc/client
LDFLAGS = -lssl -lcrypto

# Define paths for the source, object, and binary outputs for client
CLIENT_SRC_DIR = src/client
CLIENT_OBJ_DIR = build/obj/client
CLIENT_BIN_DIR = build/bin
CLIENT_EXEC = $(CLIENT_BIN_DIR)/client_app

# Define paths for the source, object, and binary outputs for server
SERVER_SRC_DIR = src/server
SERVER_OBJ_DIR = build/obj/server
SERVER_BIN_DIR = build/bin
SERVER_EXEC = $(SERVER_BIN_DIR)/server_app

# Source and object file associations for client
CLIENT_SOURCES = $(wildcard $(CLIENT_SRC_DIR)/*.c)
CLIENT_OBJECTS = $(patsubst $(CLIENT_SRC_DIR)/%.c,$(CLIENT_OBJ_DIR)/%.o,$(CLIENT_SOURCES))

# Source and object file associations for server
SERVER_SOURCES = $(wildcard $(SERVER_SRC_DIR)/*.c)
SERVER_OBJECTS = $(patsubst $(SERVER_SRC_DIR)/%.c,$(SERVER_OBJ_DIR)/%.o,$(SERVER_SOURCES))

# Default target
all: create_dirs $(CLIENT_EXEC) $(SERVER_EXEC)

# Create directories
create_dirs:
	mkdir -p $(CLIENT_OBJ_DIR) $(CLIENT_BIN_DIR) $(SERVER_OBJ_DIR) $(SERVER_BIN_DIR)

# Linking the client executable from the object files
$(CLIENT_EXEC): $(CLIENT_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compiling every C file to object file for client
$(CLIENT_OBJ_DIR)/%.o: $(CLIENT_SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Linking the server executable from the object files
$(SERVER_EXEC): $(SERVER_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS) -pthread

# Compiling every C file to object file for server
$(SERVER_OBJ_DIR)/%.o: $(SERVER_SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Clean up
clean:
	rm -rf build/obj build/bin

.PHONY: all clean create_dirs
