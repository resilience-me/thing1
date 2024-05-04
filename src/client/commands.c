#include <stdio.h>
#include <string.h>
#include <openssl/ssl.h>
#include "commands.h"
#include "command_handlers.h"
#include "command_defs.h"

// Array of function pointers to command handlers
void (*command_handlers[])(SSL*) = {
    handle_login,
    handle_register,
    handle_logout,
    handle_delete_account,
    handle_add_connection,
    NULL  // Include NULL for completeness
};

// Function to initialize commands
void initialize_commands() {
    int count = client_command_names_count();

    // Allocate memory for commands array with an extra slot for the NULL terminator
    commands = malloc((count + 1) * sizeof(Command));

    for (int i = 0; i < count; i++) {
        commands[i].name = client_command_names[i];
        commands[i].handler = (void*)command_handlers[i];
    }

    // Set the last command to NULL to mark the end
    commands[count].name = NULL;
    commands[count].handler = NULL;
}

// Constructing the commands array
Command commands[];

void cleanup_commands() {
    free(commands);  // Free the allocated memory when no longer needed
}

void dispatch_command(SSL *ssl, const char *cmd) {
    int found = 0;

    for (Command *command = commands; command->name != NULL; command++) {
        if (strcmp(cmd, command->name) == 0) {
            if (command->handler) {  // Check if handler is not NULL
                ((CommandHandler)command->handler)(ssl);
            }
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Unknown command. Please try again.\n");
    }
}
