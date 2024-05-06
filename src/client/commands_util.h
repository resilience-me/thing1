#ifndef COMMANDS_UTIL_H
#define COMMANDS_UTIL_H

#include "command_defs.h"
#include <openssl/ssl.h>

typedef void (*CommandHandler)(SSL*);

extern Command *commands;  // Makes the commands array available to other files

extern void *command_handlers[];

void dispatch_command(SSL *ssl, const char *cmd);

#endif // COMMANDS_UTIL_H
