#include "client_commands.h"  // Include the header file for declarations
#include "client_command_handlers.h"

// Command commands[] = {
//     {"LOGIN", handle_login_user},
//     {"LOGOUT", handle_logout_user},
//     {"REGISTER", handle_register_user},
//     {"DELETE_ACCOUNT", handle_delete_user},
//     {"ADD_CONNECTION", handle_add_connection},
//     {NULL, NULL}  // Terminating entry
// };

// Array of function pointers to command handlers
void *client_command_handlers[] = {
    (void*)handle_login_user,
    (void*)handle_register_user,
    (void*)handle_logout_user,
    (void*)handle_delete_user,
    (void*)handle_add_connection,
    NULL  // Include NULL for completeness
};

// Constructing the commands array
Command *client_commands = NULL;
