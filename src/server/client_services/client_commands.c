#include "client_commands.h"  // Include the header file for declarations
#include "client_command_handlers.h"

Command commands[] = {
    {"LOGIN", handle_login_user, 0},
    {"LOGOUT", handle_logout_user, 1},
    {"REGISTER", handle_register_user, 0},
    {"DELETE_ACCOUNT", handle_delete_user, 1},
    {"ADD_CONNECTION", handle_add_connection, 1},
    {NULL, NULL, 0}  // Terminating entry
};
