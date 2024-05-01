#ifndef NETWORK_H
#define NETWORK_H

#include <openssl/ssl.h>

void init_openssl();
void cleanup_openssl();
SSL_CTX *create_context();
void configure_context(SSL_CTX *ctx);
int open_connection(const char *hostname, int port);
SSL* perform_ssl_handshake(SSL_CTX *ctx, int sock);
char* parse_url(const char *url, int *port);

#endif
