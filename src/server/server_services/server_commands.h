#ifndef CLIENT_COMMANDS_H
#define CLIENT_COMMANDS_H

#include <openssl/ssl.h>  // Needed for SSL type in function prototypes
#include "session_defs.h"

typedef const char *(*CommandHandler)(Session *session, const char *args);

typedef struct {
    char *name;
    CommandHandler handler;
} Command;

extern Command server_commands[];  // Making the commands array available externally

#endif // CLIENT_COMMANDS_H
