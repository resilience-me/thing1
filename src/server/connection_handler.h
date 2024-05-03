#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include <openssl/ssl.h>
#include <pthread.h>
#include <stdint.h>

// Function prototypes
void *handle_connection(void *arg);

// Add more function prototypes for server-to-client and server-to-server commands...

#endif /* CONNECTION_HANDLER_H */
