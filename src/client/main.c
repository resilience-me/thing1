#include "connection_handler.h"
#include "parse_string_util.h"
#include "client_config.h"
#include "net_utils.h"
#include "ssl_utils.h"
#include "protocol_defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    const char *url = (argc == 2) ? argv[1] : "localhost";
    int port = CLIENT_DEFAULT_PORT;
    
    init_openssl();

    SSL_CTX *ctx = create_ssl_client_context();
    if (ctx == NULL) {
        fprintf(stderr, "Failed to initialize SSL context\n");
        return EXIT_FAILURE;
    }
    
    configure_ssl_client_context(ctx);

    char *hostname = parse_address_and_port(url, &port);
    if (!hostname) {
        fprintf(stderr, "Invalid URL provided\n");
        return EXIT_FAILURE;
    }
    
    // Open a TCP connection to the server
    int sock = open_connection(hostname, port);
    if (sock < 0) {
        fprintf(stderr, "Failed to open TCP connection\n");
        return EXIT_FAILURE;
    }

    // Send the protocol header to the server over TCP
    struct ProtocolHeader header;
    header.connectionType = CLIENT_CONNECTION;
    if (send(sock, &header, sizeof(header), 0) != sizeof(header)) {
        perror("Failed to send protocol header");
        close(sock);
        return EXIT_FAILURE;
    }

    // Set up the SSL connection using the TCP socket
    SSL *ssl = setup_ssl_client_connection(ctx, sock);
    if (!ssl) {
        fprintf(stderr, "Failed to set up SSL connection\n");
        close(sock);
        return EXIT_FAILURE;
    }

    // Read acknowledgment message from the server
    char ack_message[256]; // Adjust the buffer size as needed
    int bytes = SSL_read(ssl, ack_message, sizeof(ack_message) - 1);
    if (bytes > 0) {
        ack_message[bytes] = '\0'; // Null-terminate the received string
        printf("Server acknowledgment: %s\n", ack_message);
    } else {
        printf("Failed to receive acknowledgment from server\n");
        exit(EXIT_FAILURE); // or pthread_exit(NULL);
    }

    printf("Connected with %s encryption\n", SSL_get_cipher(ssl));

    interact_with_server(ssl);  // Start interacting with the server

    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    cleanup_openssl();
    free(hostname);
    return 0;
}
