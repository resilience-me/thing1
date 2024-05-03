#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX *create_server_context();
void configure_server_context(SSL_CTX *ctx);
int create_socket(int port);

// Function to establish an SSL connection to a remote server
SSL* establish_connection(const char *server_address, int port);

#endif /* NETWORK_H */
