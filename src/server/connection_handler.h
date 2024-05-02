#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include <openssl/ssl.h>
#include <pthread.h>
#include <stdint.h>

// Constants for connection types
#define CLIENT_CONNECTION 0x01
#define SERVER_CONNECTION 0x02

// Structure for the protocol header
struct ProtocolHeader {
    uint8_t connectionType; // Identifier field indicating connection type
    // Other fields...
};

// Function prototypes
void *handle_connection(void *arg);

// Add more function prototypes for server-to-client and server-to-server commands...

#endif /* CONNECTION_HANDLER_H */
