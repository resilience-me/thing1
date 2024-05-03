#ifndef PROTOCOL_DEFS_H
#define PROTOCOL_DEFS_H

#include <stdint.h>

// Constants for connection types
#define CLIENT_CONNECTION 0x01
#define SERVER_CONNECTION 0x02

// Structure for the protocol header
struct ProtocolHeader {
    uint8_t connectionType; // Identifier field indicating connection type
    // Other fields...
};

#endif // PROTOCOL_DEFS_H
