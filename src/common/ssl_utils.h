#ifndef SSL_UTILS_H
#define SSL_UTILS_H

#include <openssl/ssl.h>

// SSL initialization and cleanup
void init_openssl();
void cleanup_openssl();

// Create SSL context for server
SSL_CTX *create_ssl_context();
// Create SSL context for client
SSL_CTX *create_ssl_context();

// Configure SSL context
void configure_context(SSL_CTX *ctx);

// Generic SSL handshake function
SSL* ssl_client_handshake(SSL_CTX *ctx, int sock);
SSL* ssl_server_handshake(SSL_CTX *ctx, int sock);

#endif /* SSL_UTILS_H */
