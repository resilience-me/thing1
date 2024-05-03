#include "commands.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef void (*CommandHandler)(SSL*);

typedef struct {
    const char* name;
    CommandHandler handler;
} Command;

Command commands[] = {
    {"LOGIN", handle_login},
    {"REGISTER", handle_register},
    {"LOGOUT", handle_logout},
    {"DELETE_ACCOUNT", handle_delete_account},
    {"ADD_CONNECTION", handle_add_connection},
    {"EXIT", NULL},  // Include EXIT as a command for clarity in the menu
    {NULL, NULL}  // Terminator
};

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

    // Query for username
    printf("Enter username (leave empty if default account): ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';  // Remove newline character

    // Query for server address
    printf("Enter server address (leave empty if on the same server): ");
    fgets(server_address, sizeof(server_address), stdin);
    server_address[strcspn(server_address, "\n")] = '\0';  // Remove newline character

    // Query for port
    printf("Enter port (leave empty if default port): ");
    fgets(port, sizeof(port), stdin);
    port[strcspn(port, "\n")] = '\0';  // Remove newline character

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

void interact_with_server(SSL *ssl) {
    char cmd[256];
    char command_list[1024] = "Enter command (";  // Starting part of the command prompt

    // Generate command list string from the commands array
    for (Command *command = commands; command->name != NULL; command++) {
        strcat(command_list, command->name);
        if ((command + 1)->name != NULL) {
            strcat(command_list, ", ");  // Add comma only if not the last command
        }
    }
    strcat(command_list, "): ");  // Close the prompt string

    while (1) {
        printf("%s", command_list);  // Use the dynamically created list in the prompt
        if (!fgets(cmd, sizeof(cmd), stdin) || strcmp(cmd, "EXIT\n") == 0) {
            break;  // Exit loop if user types "EXIT" or there's an input error
        }

        cmd[strcspn(cmd, "\n")] = '\0';  // Remove newline character

        for (Command *command = commands; command->name != NULL; command++) {
            if (strcmp(cmd, command->name) == 0 && command->handler) {
                command->handler(ssl);
                break;  // Break since command is handled
            }
        }
    }
}
