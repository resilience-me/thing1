#include "command_defs.h"
#include "server_command_handlers.h"
#include <stddef.h>

// Array of function pointers to command handlers
void *server_as_client_command_handlers[] = {
    (void*)send_account_exists_query,
    NULL  // Include NULL for completeness
};
Command *server_as_client_commands = NULL;
