#ifndef CLIENT_COMMANDS_H
#define CLIENT_COMMANDS_H

#define SERVER_COMMAND_NAMES \
    "ACCOUNT_EXISTS", \
    NULL  // Ensure the list is properly terminated with NULL

#include "command_defs.h"
#include "session_defs.h"

typedef const char *(*ServerCommandHandler)(Session *session, const char *args);

extern Command server_commands[];  // Making the commands array available externally

#endif // CLIENT_COMMANDS_H
