#ifndef COMMANDS_H
#define COMMANDS_H

#include <openssl/ssl.h>

typedef void (*CommandHandler)(SSL*);

typedef struct {
    const char* name;
    CommandHandler handler;
} Command;

extern Command commands[];  // Makes the commands array available to other files

void dispatch_command(SSL *ssl, const char *cmd);

#endif // COMMANDS_H
