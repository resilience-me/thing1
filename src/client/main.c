#include "config.h" 
#include "commands.h"  // Assuming this includes necessary protocol or communication definitions
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "protocol_defs.h"
#include "ssl_utils.h"
#include "net_utils.h"

int main(int argc, char **argv) {
    const char *url = (argc == 2) ? argv[1] : "localhost";
    int port = DEFAULT_PORT;

    // Initialize OpenSSL
    init_openssl();
    SSL_CTX *ctx = create_ssl_context("client");
    if (!ctx) {
        fprintf(stderr, "SSL context creation failed\n");
        cleanup_openssl();
        return EXIT_FAILURE;
    }

    // Parse URL to get hostname and port
    char *hostname = parse_url(url, &port);
    if (!hostname) {
        fprintf(stderr, "Invalid URL provided\n");
        SSL_CTX_free(ctx);
        cleanup_openssl();
        return EXIT_FAILURE;
    }

    // Open socket connection
    int sock = open_socket_connection(hostname, port);
    if (sock < 0) {
        fprintf(stderr, "Failed to open socket connection\n");
        free(hostname);
        SSL_CTX_free(ctx);
        cleanup_openssl();
        return EXIT_FAILURE;
    }

    // Perform SSL handshake
    SSL *ssl = ssl_handshake(ctx, sock);
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
        perror("SSL_write failed to send protocol header");
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

    // Assume this function exists for further interaction
    interact_with_server(ssl);  // Start interacting with the server

    // Clean up resources
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    cleanup_openssl();
    free(hostname);
    return 0;
}
