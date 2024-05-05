#ifndef SERVER_AS_CLIENT_COMMANDS_H
#define SERVER_AS_CLIENT_COMMANDS_H

#include "command_defs.h"
#include <openssl/ssl.h>                // For SSL type

typedef const char *(*ServerAsClientCommandHandler)(SSL* ssl, const char *args);

extern Command *server_as_client_commands; // Making the commands array available externally

extern void *server_as_client_command_handlers[];

#endif // SERVER_AS_CLIENT_COMMANDS_H
