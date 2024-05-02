#include "network.h"
#include "connection_handler.h"
#include "database.h"
#include "config_server.h"
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
    fprintf(stderr, "Initializing OpenSSL...\n");
    init_openssl();
    
    fprintf(stderr, "Creating SSL context...\n");
    SSL_CTX *ctx = create_ssl_context(TLS_server_method());
    configure_context_server(ctx);
    
    fprintf(stderr, "Creating socket...\n");
    int sock = create_socket(PORT);

    // Accept incoming connections and handle them
    while (1) {
        fprintf(stderr, "Waiting for incoming connection...\n");
        struct sockaddr_in addr;
        uint len = sizeof(addr);
        pthread_t tid;

        int client = accept(sock, (struct sockaddr*)&addr, &len);
        if (client < 0) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }

        fprintf(stderr, "Accepted connection ");

        fprintf(stderr, "Creating thread to handle connection...\n");
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
