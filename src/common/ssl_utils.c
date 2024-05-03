#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>  // For fprintf, stderr

void init_openssl() {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl() {
    EVP_cleanup();
}

void configure_ssl_client_context(SSL_CTX *ctx) {
    SSL_CTX_set_default_verify_paths(ctx);
}

SSL_CTX* create_ssl_client_context(void) {
    const SSL_METHOD *method = TLS_client_method(); // Use TLS_client_method() as default
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL client context");
        ERR_print_errors_fp(stderr);
        return NULL; // Return NULL to indicate failure
    }

    // Add any common configuration settings here, e.g., disable deprecated protocols
    // SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);

    return ctx;
}

SSL* setup_ssl_client_connection(SSL_CTX *ctx, int sock) {
    SSL *ssl = SSL_new(ctx);
    if (!ssl) {
        ERR_print_errors_fp(stderr);
        return NULL;  // Ensure SSL struct creation didn't fail
    }

    SSL_set_fd(ssl, sock);
    if (SSL_connect(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        return NULL;  // Return NULL to indicate handshake failure
    }

    return ssl;  // Return the SSL object on success
}
