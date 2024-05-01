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

#define CERTIFICATE_FILE "/etc/letsencrypt/live/jipple.net/fullchain.pem"
#define PRIVATE_KEY_FILE "/etc/letsencrypt/live/jipple.net/privkey.pem"

void initialize_openssl();
void cleanup_openssl();
SSL_CTX *create_context();
void configure_context(SSL_CTX *ctx);
int create_socket(int port);

#endif /* NETWORK_H */