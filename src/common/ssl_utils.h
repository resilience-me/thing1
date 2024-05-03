#ifndef SSL_UTILS_H
#define SSL_UTILS_H

#include <openssl/ssl.h>

// Function to perform SSL client handshake
SSL* ssl_client_handshake(SSL_CTX *ctx, int sock);

#endif // SSL_UTILS_H
