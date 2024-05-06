#include "connection_handler.h"
#include "client_connection_handler.h"
#include "server_connection_handler.h"
#include "protocol_defs.h"

#include <stdio.h> // Add other necessary headers as needed
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/err.h>

void handle_new_connection(int client_sock, SSL_CTX *ctx) {
    struct ProtocolHeader header;
    if (recv(client_sock, &header, sizeof(header), 0) <= 0) {
        perror("Error receiving connection type");
        close(client_sock);
        return;
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_sock);

    if (header.connectionType == CLIENT_CONNECTION) {
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
        } else {
            pthread_t tid;
            if (pthread_create(&tid, NULL, handle_client_connection, ssl) != 0) {
                perror("Unable to create thread for client connection");
            }
        }
    } else if (header.connectionType == SERVER_CONNECTION) {
        SSL_set_verify(ssl, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
        } else {
            pthread_t tid;
            if (pthread_create(&tid, NULL, handle_server_connection, ssl) != 0) {
                perror("Unable to create thread for server connection");
            }
        }
    } else {
        perror("Invalid connection type");
    }

    if (!SSL_in_init(ssl)) {
        SSL_free(ssl);
        close(client_sock);
    }
}
