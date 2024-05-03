# General compilation settings
CC = gcc
CFLAGS = -Wall -g -Isrc/common -Isrc/server -Isrc/client -Isrc/server/client_services -Isrc/server/server_services
LDFLAGS = -lssl -lcrypto

# Define paths for the source, object, and binary outputs
COMMON_SRC_DIR = src/common
CLIENT_SRC_DIR = src/client
SERVER_SRC_DIR = src/server
CLIENT_SERVICES_DIR = $(SERVER_SRC_DIR)/client_services
SERVER_SERVICES_DIR = $(SERVER_SRC_DIR)/server_services
OBJ_DIR = build/obj
BIN_DIR = build/bin
COMMON_OBJ_DIR = $(OBJ_DIR)/common
CLIENT_OBJ_DIR = $(OBJ_DIR)/client
SERVER_OBJ_DIR = $(OBJ_DIR)/server
CLIENT_SERVICES_OBJ_DIR = $(SERVER_OBJ_DIR)/client_services
SERVER_SERVICES_OBJ_DIR = $(SERVER_OBJ_DIR)/server_services
CLIENT_EXEC = $(BIN_DIR)/client_app
SERVER_EXEC = $(BIN_DIR)/server_app

# Source and object file associations for common, client, and server
COMMON_SOURCES = $(wildcard $(COMMON_SRC_DIR)/*.c)
COMMON_OBJECTS = $(patsubst $(COMMON_SRC_DIR)/%.c,$(COMMON_OBJ_DIR)/%.o,$(COMMON_SOURCES))
CLIENT_SOURCES = $(wildcard $(CLIENT_SRC_DIR)/*.c)
CLIENT_OBJECTS = $(patsubst $(CLIENT_SRC_DIR)/%.c,$(CLIENT_OBJ_DIR)/%.o,$(CLIENT_SOURCES))
SERVER_SOURCES = $(wildcard $(SERVER_SRC_DIR)/*.c)
SERVER_OBJECTS = $(patsubst $(SERVER_SRC_DIR)/%.c,$(SERVER_OBJ_DIR)/%.o,$(SERVER_SOURCES))
CLIENT_SERVICES_SOURCES = $(wildcard $(CLIENT_SERVICES_DIR)/*.c)
CLIENT_SERVICES_OBJECTS = $(patsubst $(CLIENT_SERVICES_DIR)/%.c,$(CLIENT_SERVICES_OBJ_DIR)/%.o,$(CLIENT_SERVICES_SOURCES))
SERVER_SERVICES_SOURCES = $(wildcard $(SERVER_SERVICES_DIR)/*.c)
SERVER_SERVICES_OBJECTS = $(patsubst $(SERVER_SERVICES_DIR)/%.c,$(SERVER_SERVICES_OBJ_DIR)/%.o,$(SERVER_SERVICES_SOURCES))

# Default target
all: create_dirs $(CLIENT_EXEC) $(SERVER_EXEC)

create_dirs:
	mkdir -p $(COMMON_OBJ_DIR) $(CLIENT_OBJ_DIR) $(BIN_DIR) $(SERVER_OBJ_DIR) $(CLIENT_SERVICES_OBJ_DIR) $(SERVER_SERVICES_OBJ_DIR)

# Linking the client executable from the object files
$(CLIENT_EXEC): $(COMMON_OBJECTS) $(CLIENT_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Linking the server executable from the object files
$(SERVER_EXEC): $(COMMON_OBJECTS) $(SERVER_OBJECTS) $(SERVER_SERVICES_OBJECTS) $(CLIENT_SERVICES_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS) -pthread

# Compiling every C file to object file for common code
$(COMMON_OBJ_DIR)/%.o: $(COMMON_SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Compiling every C file to object file for client
$(CLIENT_OBJ_DIR)/%.o: $(CLIENT_SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Compiling every C file to object file for server
$(SERVER_OBJ_DIR)/%.o: $(SERVER_SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Compiling every C file to object file for client services
$(CLIENT_SERVICES_OBJ_DIR)/%.o: $(CLIENT_SERVICES_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Compiling every C file to object file for server services
$(SERVER_SERVICES_OBJ_DIR)/%.o: $(SERVER_SERVICES_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean create_dirs
