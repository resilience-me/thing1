#ifndef NETWORK_H
#define NETWORK_H

#include <openssl/ssl.h>

void configure_context(SSL_CTX *ctx);
char* parse_url(const char *url, int *port);

#endif
