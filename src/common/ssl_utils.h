#ifndef SSL_UTILS_H
#define SSL_UTILS_H

#include <openssl/ssl.h>

void init_openssl();
void cleanup_openssl();

void configure_client_context(SSL_CTX *ctx);

SSL_CTX* create_ssl_client_context(void);

// Function to perform SSL client handshake
SSL* ssl_client_handshake(SSL_CTX *ctx, int sock);

SSL* setup_ssl_connection(SSL_CTX *ctx, int sockfd);

#endif // SSL_UTILS_H
