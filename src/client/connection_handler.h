#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include <openssl/ssl.h>

void interact_with_server(SSL *ssl);

#endif
