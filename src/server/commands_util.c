#include "commands_util.h"
#include "server_as_server_command_handlers.h"
#include "server_as_client_command_handlers.h"
#include "client_command_handlers.h"
#include "command_defs.h"
#include <stddef.h>

// Arrays of function pointers to command handlers
void *server_as_client_command_handlers[] = {
    (void*)handle_send_account_exists_query,
    NULL  // Include NULL for completeness
};
void *server_as_server_command_handlers[] = {
    NULL,  // Handler not implemented yet
    NULL  // Include NULL for completeness
};
void *client_command_handlers[] = {
    (void*)handle_login_user,
    (void*)handle_register_user,
    (void*)handle_logout_user,
    (void*)handle_delete_user,
    (void*)handle_add_connection,
    NULL  // Include NULL for completeness
};

// Constructing the commands arrays
Command *server_as_client_commands = NULL;
Command *server_as_server_commands = NULL;
Command *client_commands = NULL;

void initialize_all_commands() {
    client_commands = initialize_commands(client_command_names, client_command_handlers);
    server_as_client_commands = initialize_commands(server_command_names, server_as_client_command_handlers);
    server_as_server_commands = initialize_commands(server_command_names, server_as_client_command_handlers);
}

void free_all_commands() {
    free(client_commands);
    free(server_as_client_commands);
    free(server_as_server_commands);
}
