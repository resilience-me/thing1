#include "commands.h"
#include "command_handlers.h"

// Array defining all command handlers
Command commands[] = {
    {"LOGIN", handle_login},
    {"REGISTER", handle_register},
    {"LOGOUT", handle_logout},
    {"DELETE_ACCOUNT", handle_delete_account},
    {"ADD_CONNECTION", handle_add_connection},
    {"EXIT", NULL},  // Include EXIT for clarity and completeness
    {NULL, NULL}    // End of array marker for iteration
};
