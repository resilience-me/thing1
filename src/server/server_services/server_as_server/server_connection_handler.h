#ifndef SERVER_CONNECTION_HANDLER_H
#define SERVER_CONNECTION_HANDLER_H

#include <openssl/ssl.h>

// Function to handle client connections
void *handle_server_connection(void *arg);

#endif /* SERVER_CONNECTION_HANDLER_H */
