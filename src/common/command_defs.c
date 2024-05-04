#include "command_defs.h"

// Function to initialize commands and return a pointer to the array
Command* initialize_client_commands() {
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
