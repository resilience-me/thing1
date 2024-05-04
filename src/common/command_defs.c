#include <stdio.h>
#include <stdlib.h>
#include "command_defs.h"  // Include the definition of Command and declarations of arrays

Command* initialize_commands(const char *command_names[], void *command_handlers[]) {
    int count = 0;
    while (command_names[count] != NULL) count++;  // Count commands

    Command *commands = malloc((count + 1) * sizeof(Command));
    if (commands == NULL) {
        fprintf(stderr, "Failed to allocate memory for commands.\n");
        return NULL;  // Early return on allocation failure
    }

    for (int i = 0; i < count; i++) {
        commands[i].name = command_names[i];
        commands[i].handler = command_handlers[i];  // Assign from passed array
    }

    commands[count].name = NULL;
    commands[count].handler = NULL;  // Mark the end
    return commands;
}

void cleanup_commands(Command *commands) {
    free(commands);  // Free the allocated memory
}
