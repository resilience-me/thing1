#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>  // For fprintf, stderr

SSL* ssl_client_handshake(SSL_CTX *ctx, int sock) {
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
