#include "commands.h"
#include <stdio.h>
#include <string.h>

void handle_login_and_register(char *loginOrRegister, SSL *ssl) {
    char username[256], password[256];
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    char request[512];
    snprintf(request, sizeof(request), "%s %s %s\n", loginOrRegister, username, password);
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
        cmd[strcspn(cmd, "\n")] = 0;
        if (strcmp(cmd, "LOGIN") == 0) {
            handle_login_and_register("LOGIN", ssl);
        } else if (strcmp(cmd, "REGISTER") == 0) {
            handle_login_and_register("REGISTER", ssl);
        } else {
            printf("Unknown command. Please try again.\n");
        }
        printf("Enter command (LOGIN, REGISTER, EXIT): ");
    }
}
