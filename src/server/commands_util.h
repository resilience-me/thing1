#ifndef COMMANDS_UTIL_H
#define COMMANDS_UTIL_H

#include "command_defs.h"
#include "session_defs.h"
#include <openssl/ssl.h>                // For SSL type

typedef const char *(*ServerAsClientCommandHandler)(SSL* ssl, const char *args);
typedef const char *(*ServerAsServerCommandHandler)(SSL* ssl, const char *args);
typedef const char *(*ClientCommandHandler)(Session *session, const char *args);

// Making the commands arrays available externally
extern Command *server_as_client_commands; 
extern Command *server_as_server_commands;
extern Command *client_commands;

// Making the commands handler lists available externally
extern void *server_as_client_command_handlers[];
extern void *server_as_server_command_handlers[];
extern void *client_command_handlers[];

void initialize_all_commands();
void free_all_commands();

#endif // COMMANDS_UTIL_H
