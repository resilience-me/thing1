#include "network.h"        // Include corresponding header first

#include "server_config.h"  // Configuration settings specific to the server
#include "net_utils.h"      // Network utility functions
#include "ssl_utils.h"      // SSL utility functions
#include "protocol_defs.h"  // Protocol header to distinguish betwween server and client connections

#include <openssl/ssl.h>
#include <openssl/x509.h>

SSL_CTX *create_ssl_server_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_ecdh_auto(ctx, 1);

    return ctx;
}

void configure_ssl_server_context(SSL_CTX *ctx) {
    SSL_CTX_use_certificate_file(ctx, CERTIFICATE_FILE, SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, PRIVATE_KEY_FILE, SSL_FILETYPE_PEM);

    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Private key does not match the public certificate\n");
        exit(EXIT_FAILURE);
    }
}

int create_socket(int port) {
    int s;
    struct sockaddr_in addr;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    // Set SO_REUSEADDR option
    int optval = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("Unable to set SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(s, 5) < 0) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }

    return s;
}

// Function to establish a connection to a remote server
SSL* establish_connection(const char *server_address, int port) {
    init_openssl();

    SSL_CTX *ctx = global_client_ctx; // Use global client context
    if (ctx == NULL) {
        fprintf(stderr, "Global client SSL context not initialized\n");
        return NULL;
    }

    int sockfd = open_connection(server_address, port);
    if (sockfd == -1) {
        return NULL;
    }

    SSL *ssl = setup_ssl_client_connection(ctx, sockfd);
    if (ssl == NULL) {
        close(sockfd);
        return NULL;
    }

    // Optionally, perform post-handshake checks or configurations here
    printf("SSL Handshake successful with %s encryption\n", SSL_get_cipher(ssl));

    // Construct and send the protocol header
    struct ProtocolHeader header;
    header.connectionType = SERVER_CONNECTION;
    if (SSL_write(ssl, &header, sizeof(header)) != sizeof(header)) {
        perror("Protocol header send failed");
        close(sockfd);
        SSL_free(ssl);
        return NULL;
    }
    return ssl;
}

// Function to verify the hostname against the SSL certificate
int verify_hostname(SSL* ssl, const char* expected_hostname) {
    X509* cert = SSL_get_peer_certificate(ssl);
    if (!cert) {
        // Certificate not present
        return 0;
    }

    // Obtain the Common Name (CN) from the certificate
    X509_NAME* subject = X509_get_subject_name(cert);
    if (subject) {
        char cn[256];
        if (X509_NAME_get_text_by_NID(subject, NID_commonName, cn, sizeof(cn)) > 0) {
            // Compare the CN with the expected hostname
            if (strcasecmp(expected_hostname, cn) == 0) {
                // Hostname matches CN
                X509_free(cert);
                return 1;
            }
        }
    }

    // No match found
    X509_free(cert);
    return 0;
}
