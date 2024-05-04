#ifndef CLIENT_COMMANDS_H
#define CLIENT_COMMANDS_H

#include "command_defs.h"
#include "session_defs.h"

typedef const char *(*CommandHandler)(Session *session, const char *args);

extern Command *commands;  // Making the commands array available externally

extern void *client_command_handlers[];

#endif // CLIENT_COMMANDS_H
