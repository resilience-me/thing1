#include "network.h"
#include "connection_handler.h"
#include "database.h"
#include "config.h"
#include "ssl_utils.h"

int main(int argc, char **argv) {
    // Create necessary directories if they do not exist
    initialize_database_directories();
    
    // Initialize OpenSSL and create socket
    int sock;
    SSL_CTX *ctx;
    init_openssl();
    ctx = create_ssl_context(TLS_server_method);
    configure_context(ctx);

    sock = create_socket(PORT);

    // Accept incoming connections and handle them
    while (1) {
        struct sockaddr_in addr;
        uint len = sizeof(addr);
        SSL *ssl;
        pthread_t tid;

        int client = accept(sock, (struct sockaddr*)&addr, &len);
        if (client < 0) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);

        if (pthread_create(&tid, NULL, handle_connection, ssl) != 0) {
            perror("Unable to create thread");
            SSL_free(ssl);
            close(client);
            continue;
        }
    }

    close(sock);
    SSL_CTX_free(ctx);
    cleanup_openssl();
    return 0;
}
