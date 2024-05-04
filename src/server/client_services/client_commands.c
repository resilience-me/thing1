#include "client_commands.h"  // Include the header file for declarations
#include "client_command_handlers.h"

Command commands[] = {
    {"LOGIN", handle_login_user},
    {"LOGOUT", handle_logout_user},
    {"REGISTER", handle_register_user},
    {"DELETE_ACCOUNT", handle_delete_user},
    {"ADD_CONNECTION", handle_add_connection},
    {NULL, NULL, 0}  // Terminating entry
};
