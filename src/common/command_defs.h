#ifndef COMMAND_DEFS_H
#define COMMAND_DEFS_H

extern const char *client_command_names[];

// Generic command structure applicable to both client and server
typedef struct {
    const char *name;
    void *handler;  // Generic pointer to any function, allows for flexible handling
} Command;

Command* initialize_commands(const char *command_names[], void *command_handlers[]);

#endif // COMMAND_DEFS_H
