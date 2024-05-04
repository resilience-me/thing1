#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include <openssl/ssl.h>

void dispatch_command(SSL *ssl, const char *cmd);

#endif
