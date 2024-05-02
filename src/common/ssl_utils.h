#ifndef SSL_UTILS_H
#define SSL_UTILS_H

#include <openssl/ssl.h>

// SSL initialization and cleanup
void init_openssl();
void cleanup_openssl();

// Create SSL context for client or server
SSL_CTX *create_ssl_context(const SSL_METHOD *method);

// Configure SSL context
void configure_context(SSL_CTX *ctx);

// Generic SSL handshake function
SSL* ssl_handshake(SSL_CTX *ctx, int sock);

#endif /* SSL_UTILS_H */
