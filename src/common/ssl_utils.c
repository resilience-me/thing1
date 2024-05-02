#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include <netdb.h>
#include <regex.h>

// General SSL initialization and cleanup
void init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl() {
    EVP_cleanup();
}

// Create SSL context for both client and server
SSL_CTX *create_ssl_context(const char *method_type) {
    const SSL_METHOD *method;

    if (strcmp(method_type, "client") == 0) {
        method = TLS_client_method();
    } else if (strcmp(method_type, "server") == 0) {
        method = TLS_server_method();  // Adjust according to your server method
    } else {
        fprintf(stderr, "Invalid method type specified\n");
        return NULL;
    }

    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_default_verify_paths(ctx);
    return ctx;
}

// Universal function to establish a socket connection
int open_socket_connection(const char *hostname, int port) {
    struct addrinfo hints, *res, *p;
    int sock;
    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, port_str, &hints, &res) != 0) {
        perror("getaddrinfo failed");
        exit(EXIT_FAILURE);
    }

    for (p = res; p != NULL; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == -1) continue;
        if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) break;
        close(sock);
    }

    freeaddrinfo(res);
    if (p == NULL) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    return sock;
}

// Generic SSL handshake function
SSL* ssl_handshake(SSL_CTX *ctx, int sock) {
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        exit(EXIT_FAILURE);
    }

    return ssl;
}

char* parse_url(const char *url, int *port) {
    regex_t regex;
    regmatch_t pmatch[4];
    const char *pattern = "^([^:/]+)(:([0-9]+))?/?$";
    char *hostname = NULL;

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        fprintf(stderr, "Could not compile regex\n");
        return NULL;
    }

    if (regexec(&regex, url, 4, pmatch, 0) == 0) {
        hostname = strndup(url + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
        if (pmatch[3].rm_so != -1) {
            *port = atoi(url + pmatch[3].rm_so);
        }
    } else {
        fprintf(stderr, "URL format error\n");
    }

    regfree(&regex);
    return hostname;
}
