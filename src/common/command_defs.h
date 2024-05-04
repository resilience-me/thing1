#ifndef COMMAND_DEFS_H
#define COMMAND_DEFS_H

// Generic command structure applicable to both client and server
typedef struct {
    char *name;
    void *handler;  // Generic pointer to any function, allows for flexible handling
} Command;

#endif // COMMAND_DEFS_H
