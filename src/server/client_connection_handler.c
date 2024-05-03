#include "client_commands.h"
#include "client_connection_handler.h"
#include "database.h"
#include "server_config.h"
#include <openssl/ssl.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *command_name;
    void (*handler)(Session *, char **args);
    int needs_authentication;
} Command;

Command commands[] = {
    {"LOGIN", login_user, 0},
    {"LOGOUT", logout_user, 1},
    {"REGISTER", register_user, 0},
    {"DELETE_ACCOUNT", delete_user, 1},
    {"ADD_CONNECTION", add_connection, 1},
    {NULL, NULL, 0}
};

void dispatch_command(Session *session, const char *command, char **args) {
    for (int i = 0; commands[i].command_name != NULL; i++) {
        if (strcmp(command, commands[i].command_name) == 0) {
            if (commands[i].needs_authentication && !session->authenticated) {
                SSL_write(session->ssl, "AUTH_REQUIRED", strlen("AUTH_REQUIRED"));
            } else {
                commands[i].handler(session, args);
            }
            return;
        }
    }
    SSL_write(session->ssl, "INVALID_COMMAND", strlen("INVALID_COMMAND"));
}

void handle_client_connection(SSL *ssl) {
    const char *ack_message = "Client connection established";
    SSL_write(ssl, ack_message, strlen(ack_message));

    Session session = {0, 0, ssl};
    const int read_size = 256;
    char buffer[read_size];
    int bytes;

    while (1) {
        bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';

        // Find the end of the first word
        char *end_of_command = strchr(buffer, ' ');
        char *command = buffer;
        char *arguments = NULL;

        if (end_of_command) {
            *end_of_command = '\0';  // Null-terminate the command
            arguments = end_of_command + 1;  // Arguments start after the space
        }

        // Dispatch command with the whole arguments string
        dispatch_command(&session, command, arguments);
    }
}
