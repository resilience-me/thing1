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

// Constructing the commands array
Command *commands = NULL;

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
