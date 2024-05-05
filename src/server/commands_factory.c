#include "commands_factory.h"
#include "client_commands.h"
#include "server_commands.h"
#include "server_as_client_commands.h"
#include "server_as_server_commands.h"

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
