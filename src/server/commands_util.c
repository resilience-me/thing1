#include "commands_util.h"
#include "server_as_server_command_handlers.h"
#include "server_as_client_command_handlers.h"
#include "client_command_handlers.h"
#include "command_defs.h"
#include "session_defs.h"
#include <stddef.h>

const char *server_command_names[] = { SERVER_COMMAND_NAMES };

// Arrays of function pointers to command handlers
void *server_as_client_command_handlers[] = {
    (void*)handle_send_account_exists_query,
    (void*)handle_set_trustline_server,
    NULL  // Include NULL for completeness
};
void *server_as_server_command_handlers[] = {
    NULL,  // Handler not implemented yet
    NULL,  // Handler not implemented yet
    NULL  // Include NULL for completeness
};
void *client_command_handlers[] = {
    (void*)handle_login_user,
    (void*)handle_register_user,
    (void*)handle_logout_user,
    (void*)handle_delete_user,
    (void*)handle_add_connection,
    (void*)handle_set_trustline,
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

void server_as_server_dispatch_command(SSL *ssl, const char *command, const char *args) {
    void *handler = find_command_handler(command, server_as_server_commands);
    if (handler) {
        const char *result = ((ServerAsServerCommandHandler)handler)(ssl, args);
        SSL_write(ssl, result, strlen(result));  // Send result back to the other server
    } else {
        SSL_write(ssl, "INVALID_COMMAND", strlen("INVALID_COMMAND"));
    }
}

const char* server_as_client_dispatch_command(SSL *ssl, const char *command, const char *args) {
    void *handler = find_command_handler(command, server_as_client_commands);
    if (handler) {
        return ((ServerAsClientCommandHandler)handler)(ssl, args);  // Execute the handler, assuming it exists
    }
    return NULL;  // Return NULL or some appropriate error handling code
}

void client_dispatch_command(SSL *ssl, Session *session, const char *command, const char *args) {
    void *handler = find_command_handler(command, client_commands);
    if (handler) {
        const char *result = ((ClientCommandHandler)handler)(session, args);
        SSL_write(ssl, result, strlen(result)); // Send result back to client
    } else {
        SSL_write(ssl, "INVALID_COMMAND", strlen("INVALID_COMMAND"));
    }
}
