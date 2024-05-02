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
SSL_CTX *create_ssl_context(const SSL_METHOD *method) {
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}


// Custom callback function for SSL handshake debug information
void ssl_handshake_info_callback(const SSL *ssl, int where, int ret) {
    if (where & SSL_CB_HANDSHAKE_START) {
        fprintf(stderr, "SSL handshake started\n");
    } else if (where & SSL_CB_HANDSHAKE_DONE) {
        fprintf(stderr, "SSL handshake completed successfully\n");
    }
}

// Generic SSL handshake function
SSL* ssl_handshake(SSL_CTX *ctx, int sock, int (*handshake_func)(SSL *)) {
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    // Set up debug callback for SSL handshake
    SSL_CTX_set_info_callback(ctx, ssl_handshake_info_callback);

    if ((*handshake_func)(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        exit(EXIT_FAILURE);
    }

    return ssl;
}
