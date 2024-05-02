#include "connection_handler.h"
#include "client_connection_handler.h"
#include "server_connection_handler.h"

#include <stdio.h> // Add other necessary headers as needed
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

void *handle_connection(void *arg) {
    ThreadArgs *thread_args = (ThreadArgs *)arg;

    // Perform SSL handshake
    SSL *ssl = ssl_handshake(thread_args->ctx, thread_args->sock, SSL_accept);
    if (!ssl) {
        fprintf(stderr, "SSL handshake failed\n");
        // Handle error appropriately, such as closing socket and freeing SSL context
        close(sock);
        SSL_CTX_free(ctx);
        pthread_exit(NULL);
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
