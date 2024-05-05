#include "commands.h"
#include "command_handlers.h"
#include "command_defs.h"

// Array of function pointers to command handlers
void *command_handlers[] = {
    (void*)handle_login,
    (void*)handle_register,
    (void*)handle_logout,
    (void*)handle_delete_account,
    (void*)handle_add_connection,
    (void*)handle_set_trustline,
    NULL  // Include NULL for completeness
};

// Constructing the commands array
Command *commands = NULL;
