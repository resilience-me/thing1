#include "commands.h"
#include <stdio.h>
#include <string.h>

void handle_login(SSL *ssl) {
    char username[256], password[256];
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    char request[512];
    snprintf(request, sizeof(request), "LOGIN %s %s\n", username, password);
    SSL_write(ssl, request, strlen(request));

    char response[1024];
    int bytes = SSL_read(ssl, response, sizeof(response) - 1);
    response[bytes] = '\0';
    printf("Server response: %s\n", response);
}

void handle_register(SSL *ssl) {
    char username[256], email[256], password[256];
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    char request[1024];
    snprintf(request, sizeof(request), "REGISTER %s %s %s\n", username, password);
    SSL_write(ssl, request, strlen(request));

    char response[1024];
    int bytes = SSL_read(ssl, response, sizeof(response) - 1);
    response[bytes] = '\0';
    printf("Server response: %s\n", response);
}

void interact_with_server(SSL *ssl) {
    char cmd[256];
    printf("Enter command (LOGIN, REGISTER, EXIT): ");
    while (fgets(cmd, sizeof(cmd), stdin) && strcmp(cmd, "EXIT\n") != 0) {
        cmd[strcspn(cmd, "\n")] = 0;  // Remove the newline character
        if (strcmp(cmd, "LOGIN") == 0) {
            handle_login(ssl);
        } else if (strcmp(cmd, "REGISTER") == 0) {
            handle_register(ssl);
        } else {
            printf("Unknown command. Please try again.\n");
        }
        printf("Enter command (LOGIN, REGISTER, EXIT): ");
    }
}
