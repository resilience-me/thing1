#ifndef CLIENT_COMMAND_HANDLERS_H
#define CLIENT_COMMAND_HANDLERS_H

#include "session_defs.h"
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
const char *client_handle_register_user(Session *session, const char *args);

// Function to add a connection to the server
const char *client_handle_add_connection(Session *session, const char *args);

// Function to delete a user
const char *client_handle_delete_user(Session *session, const char *args);

// Function to log in a user
const char *client_handle_login_user(Session *session, const char *args);

// Function to log out a user
const char *client_handle_logout_user(Session *session, const char *args);

const char *client_handle_set_trustline_client(Session *session, const char *args);

#endif /* CLIENT_COMMAND_HANDLERS_H */
