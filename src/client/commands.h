#ifndef COMMANDS_H
#define COMMANDS_H

#include "command_defs.h"
#include <openssl/ssl.h>

typedef void (*CommandHandler)(SSL*);

extern Command commands[];  // Makes the commands array available to other files

void dispatch_command(SSL *ssl, const char *cmd);
void initialize_commands();
void cleanup_commands();

#endif // COMMANDS_H
