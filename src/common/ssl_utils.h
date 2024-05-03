#ifndef SSL_UTILS_H
#define SSL_UTILS_H

#include <openssl/ssl.h>

void init_openssl();
void cleanup_openssl();

void configure_ssl_client_context(SSL_CTX *ctx);

SSL_CTX* create_ssl_client_context(void);

// Function to perform SSL client handshake
SSL* setup_ssl_client_connection(SSL_CTX *ctx, int sock);

#endif // SSL_UTILS_H
