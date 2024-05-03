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

    printf("%s", command_list);  // Use the dynamically created list in the prompt

    while (fgets(cmd, sizeof(cmd), stdin) && strcmp(cmd, "EXIT\n") != 0) {
        cmd[strcspn(cmd, "\n")] = '\0';  // Strip newline
        
        dispatch_command(ssl, cmd);  // Delegate command handling to the dispatch function

        printf("%s", command_list);  // Use the dynamically created list in the prompt
    }
}
