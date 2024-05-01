#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>

void *handle_connection(void *arg);

#endif /* COMMANDS_H */
