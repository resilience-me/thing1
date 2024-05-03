#include "client_commands.h"  // Include the header file for declarations
#include "client_command_handlers.h"

Command commands[] = {
    {"LOGIN", login_user, 0},
    {"LOGOUT", logout_user, 1},
    {"REGISTER", register_user, 0},
    {"DELETE_ACCOUNT", delete_user, 1},
    {"ADD_CONNECTION", add_connection, 1},
    {NULL, NULL, 0}  // Terminating entry
};
