#include "network.h"
#include "connection_handler.h"
#include "database.h"
#include "config.h"
#include "ssl_utils.h"
#include "thread_utils.h"

ThreadArgs *create_thread_args(SSL_CTX *ctx, int sock) {
    ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
    if (args == NULL) {
        perror("Failed to allocate memory for ThreadArgs");
        exit(EXIT_FAILURE);
    }
    args->ctx = ctx;
    args->sock = sock;
    return args;
}

int main(int argc, char **argv) {
    // Create necessary directories if they do not exist
    initialize_database_directories();
    
    // Initialize OpenSSL and create socket
    int sock;
    SSL_CTX *ctx;
    init_openssl();
    ctx = create_ssl_context(TLS_server_method());
    configure_context(ctx);

    sock = create_socket(PORT);

    // Accept incoming connections and handle them
    while (1) {
        struct sockaddr_in addr;
        uint len = sizeof(addr);
        pthread_t tid;

        int client = accept(sock, (struct sockaddr*)&addr, &len);
        if (client < 0) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }

        if (pthread_create(&tid, NULL, handle_connection, create_thread_args(ctx, sock)) != 0) {
            perror("Unable to create thread");
            close(client);
            continue;
        }
    }

    close(sock);
    SSL_CTX_free(ctx);
    cleanup_openssl();
    return 0;
}
