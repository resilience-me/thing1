#ifndef CLIENT_COMMAND_HANDLERS_H
#define CLIENT_COMMAND_HANDLERS_H

#include "client_connection_handler.h"
#include <stdio.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

// Function to handle user registration
const char *register_user(const char *username, const char *password);

// Function to add a connection to the server
const char *add_connection(Session *session, const char *username, const char *server_address, const char *portStr);

const char *delete_user(Session *session);
const char *login_user(const char *username, const char *password);
const char *logout_user(const char *username, const char *password);

#endif /* CLIENT_COMMAND_HANDLERS_H */
