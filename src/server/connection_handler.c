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

    struct ProtocolHeader header;

    // Read the connection type message from the client

    if (recv(SSL_get_fd(ssl), &header, sizeof(header), 0) <= 0) {
        perror("Error receiving connection type");
        goto cleanup;
    }

    // Determine connection type based on the identifier
    if (header.connectionType == CLIENT_CONNECTION) {
        // Perform SSL handshake
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            goto cleanup;
        }
        handle_client_connection(ssl);
    } else if (header.connectionType == SERVER_CONNECTION) {
        // Set up SSL context to enforce client certificate verification
        SSL_set_verify(ssl, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
        // Perform SSL handshake
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            goto cleanup;
        }
        handle_server_connection(ssl);
    } else {
        // Invalid connection type
    }

cleanup:
    close(SSL_get_fd(ssl));
    SSL_free(ssl);
    pthread_exit(NULL);
}
