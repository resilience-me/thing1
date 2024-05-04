#include <stdio.h>
#include <string.h>
#include <openssl/ssl.h>
#include "commands.h"
#include "command_handlers.h"
#include "command_defs.h"

// Array of function pointers to command handlers
void *command_handlers[] = {
    (void*)handle_login,
    (void*)handle_register,
    (void*)handle_logout,
    (void*)handle_delete_account,
    (void*)handle_add_connection,
    NULL  // Include NULL for completeness
};

// Function to initialize commands and return a pointer to the array
Command* initialize_commands() {
    int count = 0;
    while (command_names[count] != NULL) count++;  // Count commands

    Command *commands = malloc((count + 1) * sizeof(Command));
    if (commands == NULL) {
        fprintf(stderr, "Failed to allocate memory for commands.\n");
        return NULL; // Early return on allocation failure
    }

    for (int i = 0; i < count; i++) {
        commands[i].name = command_names[i];
        commands[i].handler = command_handlers[i];  // Assign as void* directly
    }

    commands[count].name = NULL;
    commands[count].handler = NULL;  // Mark the end
    return commands;
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
