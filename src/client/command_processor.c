#include "command_processor.h"
#include "command_handlers.h"

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
