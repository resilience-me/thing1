#include "client_commands.h"
#include "client_connection_handler.h"
#include "database.h"
#include "server_config.h"
#include <openssl/ssl.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void handle_client_connection(SSL *ssl) {

    // Send acknowledgment to the client
    const char *ack_message = "Client connection established";
    SSL_write(ssl, ack_message, strlen(ack_message));

    Session session = {0};  // Initializes username and authenticated status
    
    const int read_size = 256;
    char buffer[read_size];
    int bytes;

    while (1) {
        bytes = SSL_read(ssl, buffer, sizeof(buffer));
        if (bytes <= 0) break;  // No more data or error occurred
        buffer[bytes] = '\0';  // Ensure null termination

        char *token = strtok(buffer, " ");
        if (token == NULL) continue;  // Skip empty input

        if (!session.authenticated) {
            if (strcmp(token, "LOGIN") == 0) {
                char *username = strtok(NULL, " ");
                char *password = strtok(NULL, " ");
                if (username && password) {
                    const char *login_result = login_user(username, password);
                    if (strcmp(login_result, "LOGIN_SUCCESS") == 0) {
                        strncpy(session.username, username, sizeof(session.username)-1);
                        session.authenticated = 1;
                    }
                    SSL_write(ssl, login_result, strlen(login_result));
                } else {
                    SSL_write(ssl, "INVALID_LOGIN_COMMAND", strlen("INVALID_LOGIN_COMMAND"));
                }
            } else if (strcmp(token, "REGISTER") == 0) {
                char *username = strtok(NULL, " ");
                char *password = strtok(NULL, " ");
                if (username && password) {
                    const char *register_result = register_user(username, password);
                    SSL_write(ssl, register_result, strlen(register_result));
                } else {
                    SSL_write(ssl, "INVALID_REGISTER_COMMAND", strlen("INVALID_REGISTER_COMMAND"));
                }
            } else {
                SSL_write(ssl, "AUTH_REQUIRED", strlen("AUTH_REQUIRED"));
            }
        } else {
            // Process commands for authenticated users
            if (strcmp(token, "LOGOUT") == 0) {
                session.authenticated = 0;
                SSL_write(ssl, "LOGOUT_SUCCESS", strlen("LOGOUT_SUCCESS"));
            } else if (strcmp(token, "DELETE_ACCOUNT") == 0) {
                const char *delete_result = delete_user(&session);
                SSL_write(ssl, delete_result, strlen(delete_result));
            } else if (strcmp(token, "ADD_CONNECTION") == 0) {
                char *remote_username = strtok(NULL, " ");
                char *server_address = strtok(NULL, " ");
                char *port = strtok(NULL, " ");
                const char *add_connection_result = add_connection(&session, remote_username, server_address, port);
                SSL_write(ssl, add_connection_result, strlen(add_connection_result));
            } else {
                SSL_write(ssl, "INVALID_COMMAND", strlen("INVALID_COMMAND"));  // Could be further elaborated to handle specific commands
            }
        }
    }
}
