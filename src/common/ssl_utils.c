#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// General SSL initialization and cleanup
void init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl() {
    EVP_cleanup();
}

// Create SSL context for both client and server
SSL_CTX *create_ssl_context(const char *method_type) {
    const SSL_METHOD *method;

    if (strcmp(method_type, "client") == 0) {
        method = TLS_client_method();
    } else if (strcmp(method_type, "server") == 0) {
        method = TLS_server_method();
    } else {
        fprintf(stderr, "Invalid method type specified\n");
        return NULL;
    }

    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_default_verify_paths(ctx);
    return ctx;
}

// Generic SSL handshake function
SSL* ssl_handshake(SSL_CTX *ctx, int sock, int (*handshake_func)(SSL *)) {
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if ((*handshake_func)(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        exit(EXIT_FAILURE);
    }

    return ssl;
}
