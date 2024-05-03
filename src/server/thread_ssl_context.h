#ifndef THREAD_SSL_CONTEXT_H
#define THREAD_SSL_CONTEXT_H

#include <openssl/ssl.h>

// Define the structure for thread-specific SSL context
typedef struct {
    SSL_CTX *client_ctx;
} ThreadSSLContext;

// Function to initialize thread-specific SSL context
void init_thread_ssl_context();

// Function to create a new SSL context for the current thread
SSL_CTX* create_thread_ssl_context();

// Function to get the SSL context for the current thread
SSL_CTX* get_thread_ssl_context();

#endif /* THREAD_SSL_CONTEXT_H */
