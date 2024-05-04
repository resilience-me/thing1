#ifndef CLIENT_COMMAND_PROCESSOR_H
#define CLIENT_COMMAND_PROCESSOR_H

#include "session_defs.h"
#include <openssl/ssl.h>  // Needed for SSL type in function prototypes

void dispatch_command(SSL *ssl, Session *session, const char *command, const char *args);

#endif // CLIENT_COMMAND_PROCESSOR_H
