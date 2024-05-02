#include "network.h"
#include "commands.h"
#include "database.h"
#include "config.h"

void *handle_connection(void *arg) {
    SSL *ssl = (SSL *)arg;
    
    // Perform SSL handshake
    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        goto cleanup;
    }
    
    struct ProtocolHeader header;
    SSL_read(ssl, &header, sizeof(header));

    // Determine connection type based on the identifier
    if (header.connectionType == CLIENT_CONNECTION) {
        handle_client_connection(ssl);
    } else if (header.connectionType == SERVER_CONNECTION) {
        handle_server_connection(ssl);
    } else {
        // Invalid connection type
    }

cleanup:
    SSL_free(ssl);
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    // Create necessary directories if they do not exist
    initialize_database_directories();
    
    // Initialize OpenSSL and create socket
    int sock;
    SSL_CTX *ctx;

    initialize_openssl();
    ctx = create_context();
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
