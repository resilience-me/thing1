#include "connection_handler.h"
#include "client_connection_handler.h"
#include "server_connection_handler.h"
#include "protocol_defs.h"

#include <stdio.h> // Add other necessary headers as needed
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/err.h>

void *handle_connection(void *arg) {
    SSL *ssl = (SSL *)arg;
    
    // Perform SSL handshake
    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        goto cleanup;
    }
    
    struct ProtocolHeader header;
    SSL_read(ssl, &header, sizeof(header));

    // Determine connection type based on the identifier
    if (header.connectionType == CLIENT_CONNECTION) {
        handle_client_connection(ssl);
    } else if (header.connectionType == SERVER_CONNECTION) {
        handle_server_connection(ssl);
    } else {
        // Invalid connection type
    }

cleanup:
    SSL_free(ssl);
    pthread_exit(NULL);
}
