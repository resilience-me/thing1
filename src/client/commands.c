#include "commands.h"
#include "command_handlers.h"

// Array defining all command handlers
Command commands[] = {
    {"LOGIN", handle_login},
    {"REGISTER", handle_register},
    {"LOGOUT", handle_logout},
    {"DELETE_ACCOUNT", handle_delete_account},
    {"ADD_CONNECTION", handle_add_connection},
    {"EXIT", NULL},  // Include EXIT for clarity and completeness
    {NULL, NULL}    // End of array marker for iteration
};

void dispatch_command(SSL *ssl, const char *cmd) {
    int found = 0;

    for (Command *command = commands; command->name != NULL; command++) {
        if (strcmp(cmd, command->name) == 0) {
            command->handler(ssl);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Unknown command. Please try again.\n");
    }
}
