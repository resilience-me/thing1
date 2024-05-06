#include "command_handlers.h"
#include "parse_string_util.h"
#include "format_utils.h"
#include <stdio.h>
#include <string.h>

void handle_login_and_register(char *loginOrRegister, SSL *ssl) {
    char username[256], password[256];
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    // Check if username is valid
    if (!isValidUsername(username)) {
        printf("Username contains invalid characters. Please try again.\n");
        return;
    }

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';

    char request[1024];
    snprintf(request, sizeof(request), "%s %s %s\n", loginOrRegister, username, password);
    SSL_write(ssl, request, strlen(request));

    char response[1024];
    int bytes = SSL_read(ssl, response, sizeof(response) - 1);
    response[bytes] = '\0';
    printf("Server response: %s\n", response);
}

void handle_login(SSL *ssl) {
    handle_login_and_register("LOGIN", ssl);
}
void handle_register(SSL *ssl) {
    handle_login_and_register("REGISTER", ssl);
}

void handle_logout(SSL *ssl) {
    const char *logout_cmd = "LOGOUT";
    // Sending the LOGOUT command to the server
    SSL_write(ssl, logout_cmd, strlen(logout_cmd));

    // Receiving response from the server
    char response[256];
    int bytes = SSL_read(ssl, response, sizeof(response) - 1);
    if (bytes > 0) {
        response[bytes] = '\0';  // Null-terminate the response
        printf("Server response: %s\n", response);
    } else if (bytes == 0) {
        printf("Connection closed by server.\n");
    } else {
        printf("SSL read error.\n");
    }
}

void handle_delete_account(SSL *ssl) {
    printf("Confirm delete by re-entering your username: ");
    char username[256];
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';  // Remove newline

    const char *delete_cmd = "DELETE_ACCOUNT";
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "%s %s", delete_cmd, username);
    SSL_write(ssl, cmd, strlen(cmd));

    char response[256];
    int bytes = SSL_read(ssl, response, sizeof(response) - 1);
    if (bytes > 0) {
        response[bytes] = '\0';
        printf("Server response: %s\n", response);
    } else if (bytes == 0) {
        printf("Connection closed by server.\n");
    } else {
        printf("SSL read error.\n");
    }
}

void handle_add_connection(SSL *ssl) {
    char username[256];
    char server_address[256];
    char port[256];

    query_account_details(username, server_address, port);

    // Send the ADD_CONNECTION command with the parsed username, server address, and port to the server
    char command[512];
    snprintf(command, sizeof(command), "ADD_CONNECTION %s %s %s", 
             username[0] ? username : "", 
             server_address[0] ? server_address : "", 
             port[0] ? port : "");
    SSL_write(ssl, command, strlen(command));

    // Wait for and print the server's response
    char response[256];
    int bytes_received = SSL_read(ssl, response, sizeof(response) - 1); // Leave space for null terminator
    if (bytes_received > 0) {
        response[bytes_received] = '\0'; // Null-terminate the string
        printf("Server response: %s\n", response);
    } else {
        printf("Failed to receive server response.\n");
    }
}

void query_account_details(char* username, char* server_address, char* port) {
    // Query for username
    printf("Enter username (leave empty if default account): ");
    fgets(username, 256, stdin);
    username[strcspn(username, "\n")] = '\0';  // Remove newline character

    // Query for server address
    printf("Enter server address (leave empty if on the same server): ");
    fgets(server_address, 256, stdin);
    server_address[strcspn(server_address, "\n")] = '\0';  // Remove newline character

    // Query for port
    printf("Enter port (leave empty if default port): ");
    fgets(port, 256, stdin);
    port[strcspn(port, "\n")] = '\0';  // Remove newline character
}

void handle_set_trustline(SSL *ssl) {
    char username[256];
    char server_address[256];
    char port[256];

    query_account_details(username, server_address, port);    
}
