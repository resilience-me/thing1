#ifndef CLIENT_CONNECTION_HANDLER_H
#define CLIENT_CONNECTION_HANDLER_H

#include <openssl/ssl.h>

// Function to handle client connections
void handle_client_connection(SSL *ssl);

#endif /* CLIENT_CONNECTION_HANDLER_H */