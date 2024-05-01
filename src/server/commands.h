#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// Function to handle user registration
const char *register_user(const char *username, const char *password);

void *handle_connection(void *arg);

#endif /* COMMANDS_H */
