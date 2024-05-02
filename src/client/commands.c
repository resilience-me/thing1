#include "commands.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Function to check if a username contains only valid characters
int isValidUsername(const char *username) {
    for (int i = 0; username[i] != '\0'; ++i) {
        if (!(isalnum(username[i]) || username[i] == '_')) {
            return 0; // Invalid username
        }
    }
    return 1; // Valid username
}

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

void interact_with_server(SSL *ssl) {
    char cmd[256];
    printf("Enter command (LOGIN, REGISTER, EXIT): ");
    while (fgets(cmd, sizeof(cmd), stdin) && strcmp(cmd, "EXIT\n") != 0) {
        cmd[strcspn(cmd, "\n")] = '\0';
        if (strcmp(cmd, "LOGIN") == 0) {
            handle_login(ssl);
        } else if (strcmp(cmd, "REGISTER") == 0) {
            handle_register(ssl);
        } else if (strcmp(cmd, "LOGOUT") == 0) {
            handle_logout(ssl);
        } else {
            printf("Unknown command. Please try again.\n");
        }
        printf("Enter command (LOGIN, REGISTER, EXIT): ");
    }
}
