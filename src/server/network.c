#include "network.h"
#include "server_config.h"

#include "net_utils.h"
#include "ssl_utils.h"
#include "protocol_defs.h"

SSL_CTX *create_ssl_server_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_ecdh_auto(ctx, 1);

    return ctx;
}

void configure_ssl_server_context(SSL_CTX *ctx) {
    SSL_CTX_use_certificate_file(ctx, CERTIFICATE_FILE, SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, PRIVATE_KEY_FILE, SSL_FILETYPE_PEM);

    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Private key does not match the public certificate\n");
        exit(EXIT_FAILURE);
    }
}

int create_socket(int port) {
    int s;
    struct sockaddr_in addr;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    // Set SO_REUSEADDR option
    int optval = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("Unable to set SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(s, 5) < 0) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }

    return s;
}

// Function to establish a connection to a remote server
SSL* establish_connection(const char *server_address, int port) {
    init_openssl();

    SSL_CTX *ctx = global_client_ctx; // Use global client context
    if (ctx == NULL) {
        fprintf(stderr, "Global client SSL context not initialized\n");
        return NULL;
    }

    int sockfd = open_connection(server_address, port);
    if (sockfd == -1) {
        return NULL;
    }

    SSL *ssl = ssl_client_handshake(ctx, sockfd);
    if (ssl == NULL) {
        close(sockfd);
        return NULL;
    }

    // Optionally, perform post-handshake checks or configurations here
    printf("SSL Handshake successful with %s encryption\n", SSL_get_cipher(ssl));

    // Construct and send the protocol header
    struct ProtocolHeader header;
    header.connectionType = SERVER_CONNECTION;
    if (SSL_write(ssl, &header, sizeof(header)) != sizeof(header)) {
        perror("Protocol header send failed");
        close(sockfd);
        SSL_free(ssl);
        return NULL;
    }
    return ssl;
}
