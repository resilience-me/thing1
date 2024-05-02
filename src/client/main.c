#include "commands.h"  // Assuming this includes necessary protocol or communication definitions
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common/ssl_utils.h"  // Include path to shared SSL utilities
#include "common/net_utils.h"  // Include path to shared network utilities

#define DEFAULT_PORT 2012
#define CLIENT_CONNECTION 0x01

struct ProtocolHeader {
    uint8_t connectionType; // Identifier field indicating connection type
    // Other fields...
};

int main(int argc, char **argv) {
    const char *url = (argc == 2) ? argv[1] : "localhost";
    int port = DEFAULT_PORT;

    init_openssl();
    SSL_CTX *ctx = create_ssl_context("client");  // Updated to use the refactored function
    if (!ctx) {
        fprintf(stderr, "SSL context creation failed\n");
        exit(EXIT_FAILURE);
    }

    char *hostname = parse_url(url, &port);  // Ensure parse_url is implemented correctly to return the hostname
    if (!hostname) {
        fprintf(stderr, "Invalid URL provided\n");
        return EXIT_FAILURE;
    }

    int sock = open_socket_connection(hostname, port);  // Updated to use the refactored function
    if (sock < 0) {
        fprintf(stderr, "Failed to open socket connection\n");
        free(hostname);
        SSL_CTX_free(ctx);
        cleanup_openssl();
        return EXIT_FAILURE;
    }

    SSL *ssl = ssl_handshake(ctx, sock);  // Updated to use the refactored function
    if (!ssl) {
        fprintf(stderr, "SSL handshake failed\n");
        close(sock);
        SSL_CTX_free(ctx);
        cleanup_openssl();
        free(hostname);
        return EXIT_FAILURE;
    }

    // Send the protocol header to the server
    struct ProtocolHeader header;
    header.connectionType = CLIENT_CONNECTION;
    if (SSL_write(ssl, &header, sizeof(header)) < 0) {
        fprintf(stderr, "Failed to send protocol header\n");
    }

    // Read acknowledgment message from the server
    char ack_message[256]; // Adjust the buffer size as needed
    int bytes = SSL_read(ssl, ack_message, sizeof(ack_message) - 1);
    if (bytes > 0) {
        ack_message[bytes] = '\0'; // Null-terminate the received string
        printf("Server acknowledgment: %s\n", ack_message);
    } else {
        fprintf(stderr, "Failed to receive acknowledgment from server\n");
    }

    printf("Connected with %s encryption\n", SSL_get_cipher(ssl));

    // Assuming this function exists for interaction
    interact_with_server(ssl);  // Start interacting with the server

    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    cleanup_openssl();
    free(hostname);
    return 0;
}
