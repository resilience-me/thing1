#include "commands.h"

void *handle_connection(void *arg) {
    SSL *ssl = (SSL *)arg;
    const int read_size = 256;
    char buffer[read_size];
    int bytes;

    // Perform SSL handshake
    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        goto cleanup;
    }

    // Receive and process commands from the client
    while (1) {
        bytes = SSL_read(ssl, buffer, sizeof(buffer));
        if (bytes <= 0) {
            break; // No more data or error occurred
        }
        buffer[bytes] = '\0';
        printf("Received: %s\n", buffer);

        // Parse the received command
        char *token = strtok(buffer, " ");
        if (token != NULL) {
            printf("Received token: %s\n", token);

            if (strcmp(token, "LOGIN") == 0) {
                SSL_write(ssl, "LOGIN_SUCCESS", strlen("LOGIN_SUCCESS"));
            } else if (strcmp(token, "REGISTER") == 0) {
                SSL_write(ssl, "REGISTER_SUCCESS", strlen("REGISTER_SUCCESS"));
            } else {
                SSL_write(ssl, "INVALID_COMMAND", strlen("INVALID_COMMAND"));
            }
        }
    }

cleanup:
    SSL_free(ssl);
    pthread_exit(NULL);
}
