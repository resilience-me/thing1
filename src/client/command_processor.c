#include "command_processor.h"
#include "command_handlers.h"
#include "commands.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

    printf("Enter command: ");
    while (fgets(cmd, sizeof(cmd), stdin) && strcmp(cmd, "EXIT\n") != 0) {
        cmd[strcspn(cmd, "\n")] = '\0';  // Strip newline
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
        printf("Enter command: ");
    }
}
