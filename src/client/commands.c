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

// Constructing the commands array
Command commands[] = {
    // Start of the array of Command structs
    {NULL, NULL},  // Initialize the first element to ensure proper termination
    // Iterate over command names and construct Command structs
    {
        int i = 0;
        for (; command_names[i] != NULL; i++) {
            {command_names[i],  (void *)command_handlers[i]},
        }
        {NULL, NULL}  // End marker
    }
};

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
