#ifndef COMMANDS_H
#define COMMANDS_H

#include "command_defs.h"
#include <openssl/ssl.h>

typedef void (*CommandHandler)(SSL*);

extern Command *commands;  // Makes the commands array available to other files

extern void *command_handlers[];

#endif // COMMANDS_H
