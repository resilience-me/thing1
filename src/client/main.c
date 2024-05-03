#include "command_processor.h"
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
    
    configure_client_context(ctx);

    char *hostname = parse_url(url, &port);
    if (!hostname) {
        fprintf(stderr, "Invalid URL provided\n");
        return EXIT_FAILURE;
    }

    int sock = open_connection(hostname, port);
    SSL *ssl = ssl_client_handshake(ctx, sock);

    // Send the protocol header to the server
    struct ProtocolHeader header;
    header.connectionType = CLIENT_CONNECTION;
    
    SSL_write(ssl, &header, sizeof(header));

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
