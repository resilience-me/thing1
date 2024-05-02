# General compilation settings
CC = gcc
CFLAGS = -Wall -g -Isrc/client -Isrc/common
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

# Define paths for the source and object files in common directory
COMMON_SRC_DIR = common
COMMON_OBJ_DIR = build/obj/common

# Source and object file associations for client
CLIENT_SOURCES = $(wildcard $(CLIENT_SRC_DIR)/*.c) $(wildcard $(COMMON_SRC_DIR)/*.c)
CLIENT_OBJECTS = $(patsubst $(CLIENT_SRC_DIR)/%.c,$(CLIENT_OBJ_DIR)/%.o,$(CLIENT_SOURCES))

# Source and object file associations for server
SERVER_SOURCES = $(wildcard $(SERVER_SRC_DIR)/*.c) $(wildcard $(COMMON_SRC_DIR)/*.c)
SERVER_OBJECTS = $(patsubst $(SERVER_SRC_DIR)/%.c,$(SERVER_OBJ_DIR)/%.o,$(SERVER_SOURCES))

# Source and object file associations for common
COMMON_SOURCES = $(wildcard $(COMMON_SRC_DIR)/*.c)
COMMON_OBJECTS = $(patsubst $(COMMON_SRC_DIR)/%.c,$(COMMON_OBJ_DIR)/%.o,$(COMMON_SOURCES))

# Source files for SSL and network utilities
SSL_UTILS_SRC = common/ssl_utils.c
NET_UTILS_SRC = common/net_utils.c

# Object files for SSL and network utilities
SSL_UTILS_OBJ = $(COMMON_OBJ_DIR)/ssl_utils.o
NET_UTILS_OBJ = $(COMMON_OBJ_DIR)/net_utils.o

# Default target
all: create_dirs $(CLIENT_EXEC) $(SERVER_EXEC)

# Create directories
create_dirs:
	mkdir -p $(CLIENT_OBJ_DIR) $(CLIENT_BIN_DIR) $(SERVER_OBJ_DIR) $(SERVER_BIN_DIR) $(COMMON_OBJ_DIR)

# Compiling SSL and network utility source files to object files
$(SSL_UTILS_OBJ): $(SSL_UTILS_SRC)
	$(CC) -c $(CFLAGS) $< -o $@

$(NET_UTILS_OBJ): $(NET_UTILS_SRC)
	$(CC) -c $(CFLAGS) $< -o $@

# Compiling every C file to object file for common
$(COMMON_OBJ_DIR)/%.o: $(COMMON_SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Linking the client executable from the object files
$(CLIENT_EXEC): $(CLIENT_OBJECTS) $(COMMON_OBJECTS) $(SSL_UTILS_OBJ) $(NET_UTILS_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compiling every C file to object file for client
$(CLIENT_OBJ_DIR)/%.o: $(CLIENT_SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Linking the server executable from the object files
$(SERVER_EXEC): $(SERVER_OBJECTS) $(COMMON_OBJECTS) $(SSL_UTILS_OBJ) $(NET_UTILS_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) -pthread

# Compiling every C file to object file for server
$(SERVER_OBJ_DIR)/%.o: $(SERVER_SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Clean up
clean:
	rm -rf build/obj build/bin

.PHONY: all clean create_dirs
