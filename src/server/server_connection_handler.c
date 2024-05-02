#include "server_commands.h"
#include "server_connection_handler.h"
#include "database.h" // Assuming you have functions to check account existence in your database module

void handle_server_connection(SSL *ssl) {
    // Receive the command from the client
    char command[MAX_COMMAND_LENGTH];
    receive_command(ssl, command, sizeof(command));

    // Check if the received command is ACCOUNT_EXIST
    if (strcmp(command, "ACCOUNT_EXIST") == 0) {
        // Receive the username from the client
        char username[MAX_USERNAME_LENGTH];
        receive_username(ssl, username, sizeof(username));

        // Check if the account exists in the database
        if (account_exists(username)) {
            // Send the response to the client indicating that the account exists
            send_response(ssl, "ACCOUNT_EXISTS");
        } else {
            // Send the response to the client indicating that the account does not exist
            send_response(ssl, "ACCOUNT_DOES_NOT_EXIST");
        }
    } else {
        // Send an error response to the client indicating an invalid command
        send_response(ssl, "INVALID_COMMAND");
    }
}
