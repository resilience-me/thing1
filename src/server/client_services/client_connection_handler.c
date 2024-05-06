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

    printf("Handling client connection...\n");

    const char *ack_message = "Client connection established";
    if (SSL_write(ssl, ack_message, strlen(ack_message)) <= 0) {
        perror("Error sending acknowledgment message to client");
        SSL_free(ssl);
        return NULL;
    }

    printf("Acknowledgment message sent to client.\n");

    Session session = {0};
    const int read_size = 256;
    char buffer[read_size];
    int bytes;

    while (1) {
        bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';

        printf("Received command from client: %s\n", buffer);

        // Find the end of the first word
        char *end_of_command = strchr(buffer, ' ');
        char *command = buffer;
        char *arguments = NULL;

        if (end_of_command) {
            *end_of_command = '\0';  // Null-terminate the command
            arguments = end_of_command + 1;  // Arguments start after the space
        }

        printf("Dispatching command to client command handler...\n");
        // Dispatch command with the whole arguments string
        client_dispatch_command(ssl, &session, command, arguments);
    }

    printf("Closing client connection...\n");
    SSL_free(ssl);
    return NULL;
}
