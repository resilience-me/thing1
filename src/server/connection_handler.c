#include "connection_handler.h"
#include "client_connection_handler.h"
#include "server_connection_handler.h"
#include "thread_utils.h"
#include "ssl_utils.h"
#include "protocol_defs.h"

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

    printf("Thread %lu: Handling connection...\n", pthread_self());  // Feedback message

    // Perform SSL handshake
    ssl = ssl_server_handshake(thread_args->ctx, thread_args->sock);
    if (!ssl) {
        fprintf(stderr, "Thread %lu: SSL handshake failed\n", pthread_self());  // Error message
        goto cleanup;
    }

    printf("Thread %lu: SSL handshake successful\n", pthread_self());  // Feedback message

    // Handle client or server connection based on the received protocol header
    struct ProtocolHeader header;
    if (SSL_read(ssl, &header, sizeof(header)) != sizeof(header)) {
        fprintf(stderr, "Thread %lu: Error reading protocol header\n", pthread_self());  // Error message
        // Handle error appropriately
    } else {
        printf("Thread %lu: Received protocol header\n", pthread_self());  // Feedback message

        if (header.connectionType == CLIENT_CONNECTION) {
            printf("Thread %lu: Handling client connection\n", pthread_self());  // Feedback message
            handle_client_connection(ssl);
        } else if (header.connectionType == SERVER_CONNECTION) {
            printf("Thread %lu: Handling server connection\n", pthread_self());  // Feedback message
            handle_server_connection(ssl);
        } else {
            fprintf(stderr, "Thread %lu: Invalid connection type\n", pthread_self());  // Error message
            // Handle error appropriately
        }
    }

    // Clean up SSL resources
    SSL_free(ssl);
cleanup:
    close(thread_args->sock);
    SSL_CTX_free(thread_args->ctx);
    free(thread_args);

    printf("Thread %lu: Connection handling complete\n", pthread_self());  // Feedback message
    pthread_exit(NULL);
}

