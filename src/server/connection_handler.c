#include "connection_handler.h"
#include "client_connection_handler.h"
#include "server_connection_handler.h"
#include "thread_utils.h"
#include "ssl_utils.h"

#include <stdio.h> // Add other necessary headers as needed
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

void *handle_connection(void *arg) {
    ThreadArgs *thread_args = (ThreadArgs *)arg;
    SSL *ssl = NULL;

    // Perform SSL handshake
    ssl = ssl_handshake(thread_args->ctx, thread_args->sock, SSL_accept);
    if (!ssl) {
        fprintf(stderr, "SSL handshake failed\n");
        goto cleanup;
    }
    
    // Handle client or server connection based on the received protocol header
    struct ProtocolHeader header;
    if (SSL_read(ssl, &header, sizeof(header)) != sizeof(header)) {
        fprintf(stderr, "Error reading protocol header\n");
        // Handle error appropriately
    } else {
        if (header.connectionType == CLIENT_CONNECTION) {
            handle_client_connection(ssl);
        } else if (header.connectionType == SERVER_CONNECTION) {
            handle_server_connection(ssl);
        } else {
            // Invalid connection type
        }
    }

    // Clean up SSL resources
    SSL_free(ssl);
    cleanup:
        close(thread_args->sock);
        SSL_CTX_free(thread_args->ctx);
        free(thread_args);
        pthread_exit(NULL);
}
