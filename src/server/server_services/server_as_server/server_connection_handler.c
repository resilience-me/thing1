#include "server_command_handlers.h"
#include "server_connection_handler.h"
#include "database.h" // Assuming you have functions to check account existence in your database module
#include <string.h>

// Function to send a response to the client
void send_response(SSL *ssl, const char *response) {
    // Send the response to the client
    SSL_write(ssl, response, strlen(response));
}

void handle_server_connection(SSL *ssl) {
    // Receive the command from the other server
    const int read_size = 256;
    char buffer[read_size];
    int bytes = SSL_read(ssl, buffer, sizeof(buffer));
    buffer[bytes] = '\0';  // Ensure null termination
    
    char *token = strtok(buffer, " ");

    if(token == NULL) send_response(ssl, "INVALID_COMMAND");

    // Check if the received command is ACCOUNT_EXIST
    if (strcmp(token, "ACCOUNT_EXISTS") == 0) {
        // Receive the username from the client
        char *username = strtok(NULL, " ");
        const char *account_exists_result = account_exists(username);
        send_response(ssl, account_exists_result);
    } else {
        // Send an error response to the client indicating an invalid command
        send_response(ssl, "INVALID_COMMAND");
    }
}