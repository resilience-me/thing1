#ifndef SERVER_AS_SERVER_COMMANDS_H
#define SERVER_AS_SERVER_COMMANDS_H

#include "command_defs.h"
#include <openssl/ssl.h>                // For SSL type

typedef const char *(*ServerAsServerCommandHandler)(SSL* ssl, const char *args);

extern Command *server_as_server_commands; // Making the commands array available externally

extern void *server_as_server_command_handlers[];

#endif // SERVER_AS_SERVER_COMMANDS_H
