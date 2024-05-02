#ifndef THREAD_UTILS_H
#define THREAD_UTILS_H

#include <openssl/ssl.h>

// Define a struct to hold SSL context and socket
typedef struct {
    SSL_CTX *ctx;
    int sock;
} ThreadArgs;

#endif /* THREAD_UTILS_H */
