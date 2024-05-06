#ifndef COMMAND_DEFS_H
#define COMMAND_DEFS_H

#define ARGUMENT_DELIMITER ' '

#define CLIENT_COMMAND_NAMES \
    "LOGIN", \
    "REGISTER", \
    "LOGOUT", \
    "DELETE_ACCOUNT", \
    "ADD_CONNECTION", \
    "SET_TRUSTLINE", \
    NULL  // Ensure the list is properly terminated with NULL

extern const char *client_command_names[];

// Generic command structure applicable to both client and server
typedef struct {
    const char *name;
    void *handler;  // Generic pointer to any function, allows for flexible handling
} Command;

Command* initialize_commands(const char *command_names[], void *command_handlers[]);

void* find_command_handler(const char* command, Command* command_handlers);

#endif // COMMAND_DEFS_H
