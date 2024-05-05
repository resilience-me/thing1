#ifndef COMMANDS_FACTORY_H
#define COMMANDS_FACTORY_H

#include "client_commands.h"
#include "server_commands.h"
#include "server_as_client_commands.h"
#include "server_as_server_commands.h"

/**
 * Initializes all command structures necessary for the application.
 * This includes commands for client interaction, server as client, and server as server functionalities.
 */
void initialize_all_commands();

/**
 * Frees all allocated memory associated with commands initialized by initialize_all_commands.
 * This is necessary to prevent memory leaks during the shutdown or restart processes.
 */
void free_all_commands();

#endif // COMMANDS_FACTORY_H
