#include "server_as_client_command_handlers.h"
#include "server_connection_handler.h"
#include "commands_util.h"
#include "database.h" // Assuming you have functions to check account existence in your database module
#include <string.h>
#include <stdio.h> // Add other necessary headers as needed

// Function to send a response to the client
void send_response(SSL *ssl, const char *response) {
    // Send the response to the client
    SSL_write(ssl, response, strlen(response));
}

void *handle_server_connection(void *arg) {
    SSL *ssl = (SSL *)arg;

    printf("Handling server connection...\n");

    // Receive the command from the other server
    const int read_size = 256;
    char buffer[read_size];
    int bytes = SSL_read(ssl, buffer, sizeof(buffer));
    buffer[bytes] = '\0';  // Ensure null termination

    printf("Received command from the other server: %s\n", buffer);

    // Find the end of the first word
    char *end_of_command = strchr(buffer, ' ');
    char *command = buffer;
    char *arguments = NULL;

    if (end_of_command) {
        *end_of_command = '\0';  // Null-terminate the command
        arguments = end_of_command + 1;  // Arguments start after the space
    }

    printf("Dispatching command \"%s\" with arguments \"%s\"\n", command, arguments);

    // Dispatch command with the whole arguments string
    server_as_server_dispatch_command(ssl, command, arguments);
    return NULL;
}
