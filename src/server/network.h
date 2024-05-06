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

X509_NAME *get_peer_subject_name(SSL *ssl);

int is_localhost(const char *peer_ip);

const char *get_peer_ip(X509_NAME *subject_name);

const char *get_peer_certificate_common_name(X509_NAME *subject_name);

#endif /* NETWORK_H */
