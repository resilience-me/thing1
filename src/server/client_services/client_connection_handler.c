#include "client_connection_handler.h"
#include "commands_util.h"
#include "session_defs.h"
#include "database.h"
#include "server_config.h"
#include <openssl/ssl.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *handle_client_connection(void *arg) {
    SSL *ssl = (SSL *)arg;

    const char *ack_message = "Client connection established";
    SSL_write(ssl, ack_message, strlen(ack_message));

    Session session = {0};
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
        client_dispatch_command(ssl, &session, command, arguments);
    }
}
