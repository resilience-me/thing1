#ifndef COMMAND_DEFS_H
#define COMMAND_DEFS_H

// Define array of client command names
const char *client_command_names[] = {
    "LOGIN",
    "REGISTER",
    "LOGOUT",
    "DELETE_ACCOUNT",
    "ADD_CONNECTION",
    NULL // End marker
};

// Generic command structure applicable to both client and server
typedef struct {
    char *name;
    void *handler;  // Generic pointer to any function, allows for flexible handling
} Command;

Command* initialize_commands(const char *command_names[], void *command_handlers[]);

void cleanup_commands();

#endif // COMMAND_DEFS_H
