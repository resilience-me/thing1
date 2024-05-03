#ifndef CLIENT_COMMANDS_H
#define CLIENT_COMMANDS_H

#include <openssl/ssl.h>  // Needed for SSL type in function prototypes

typedef struct session Session;  // Forward declaration if Session isn't defined elsewhere

typedef const char *(*CommandHandler)(Session *session, char **args);

typedef struct {
    char *name;
    CommandHandler handler;
    int needs_authentication;
} Command;

extern Command commands[];  // Making the commands array available externally

void dispatch_command(SSL *ssl, Session *session, const char *command, const char **args);

#endif // CLIENT_COMMANDS_H
