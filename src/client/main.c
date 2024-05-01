#include "network.h"
#include "commands.h"
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_PORT 2012

int main(int argc, char **argv) {
    const char *url = (argc == 2) ? argv[1] : "localhost";
    int port = DEFAULT_PORT;

    init_openssl();
    SSL_CTX *ctx = create_context();
    configure_context(ctx);

    char *hostname = parse_url(url, &port);
    if (!hostname) {
        fprintf(stderr, "Invalid URL provided\n");
        return EXIT_FAILURE;
    }

    int sock = open_connection(hostname, port);
    SSL *ssl = perform_ssl_handshake(ctx, sock);

    interact_with_server(ssl);  // Start interacting with the server

    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    cleanup_openssl();
    free(hostname);
    return 0;
}
