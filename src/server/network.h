#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>       // Standard I/O
#include <stdlib.h>      // Standard library
#include <string.h>      // String operations
#include <errno.h>       // Error numbers

#include <sys/socket.h>  // Socket-specific functions
#include <netinet/in.h>  // Internet-specific functions
#include <unistd.h>      // Unix standard functions

#include <openssl/ssl.h> // OpenSSL-specific SSL/TLS functions
#include <openssl/err.h> // OpenSSL error functions

extern SSL_CTX *global_client_ctx;

SSL_CTX *create_ssl_server_context();
void configure_ssl_server_context(SSL_CTX *ctx);
int create_socket(int port);

// Function to establish an SSL connection to a remote server
SSL* establish_connection(const char *server_address, int port);

#endif /* NETWORK_H */
