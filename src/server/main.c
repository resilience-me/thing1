#include "network.h"
#include "connection_handler.h"
#include "commands_util.h"
#include "command_defs.h"
#include "database.h"
#include "server_config.h"
#include "ssl_utils.h"

SSL_CTX *global_client_ctx = NULL; // Global client SSL context

int main(int argc, char **argv) {
    // Initialization code as before
    int sock = create_socket(SERVER_DEFAULT_PORT);
    SSL_CTX *server_ctx = create_ssl_server_context();
    configure_ssl_server_context(server_ctx);

    while (1) {
        struct sockaddr_in addr;
        uint len = sizeof(addr);
        int client = accept(sock, (struct sockaddr*)&addr, &len);
        if (client < 0) {
            perror("Unable to accept");
            continue;
        }

        handle_new_connection(client, server_ctx);
    }

int main(int argc, char **argv) {
    // Create necessary directories if they do not exist
    initialize_database_directories();
    initialize_all_commands();

    // Initialize SSL contexts
    init_openssl();

    // Create and configure the global client SSL context
    global_client_ctx = create_ssl_client_context();
    configure_ssl_server_context(global_client_ctx);
    SSL_CTX_set_verify(global_client_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);

    // Create and configure the server SSL context
    SSL_CTX *server_ctx = create_ssl_server_context();
    configure_ssl_server_context(server_ctx);

    // Create and bind socket
    int sock = create_socket(SERVER_DEFAULT_PORT);

    // Accept incoming connections and handle them

    while (1) {
        struct sockaddr_in addr;
        uint len = sizeof(addr);
        int client = accept(sock, (struct sockaddr*)&addr, &len);
        if (client < 0) {
            perror("Unable to accept");
            continue;
        }

        handle_new_connection(client, server_ctx);
    }

    // Clean up SSL contexts
    SSL_CTX_free(global_client_ctx);
    SSL_CTX_free(server_ctx);

    // Clean up OpenSSL
    cleanup_openssl();

    // Close socket
    close(sock);
    free_all_commands();
    return 0;
}
