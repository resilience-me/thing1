#include "network.h"
#include "command_handler.h"

#define PORT 2012

int main(int argc, char **argv) {
    int sock;
    SSL_CTX *ctx;

    initialize_openssl();
    ctx = create_context();
    configure_context(ctx);

    sock = create_socket(PORT);

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
