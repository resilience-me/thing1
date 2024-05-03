#ifndef CLIENT_CONNECTION_HANDLER_H
#define CLIENT_CONNECTION_HANDLER_H

#include <openssl/ssl.h>

typedef struct {
    char username[256];       // Username for the session
    int authenticated;        // 0 = Not authenticated, 1 = Authenticated
} Session;

// Function to handle client connections
void handle_client_connection(SSL *ssl);

#endif /* CLIENT_CONNECTION_HANDLER_H */
