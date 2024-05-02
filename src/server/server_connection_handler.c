#include "server_commands.h"
#include "server_connection_handler.h"
#include "database.h" // Assuming you have functions to check account existence in your database module
#include <string.h>

// Function to send a response to the client
void send_response(SSL *ssl, const char *response) {
    // Send the response to the client
    SSL_write(ssl, response, strlen(response));
}

// Function to establish a connection to a remote server
SSL* establish_connection(const char *server_address, char *portStr) {
    int sockfd;
    SSL_CTX *ctx;
    SSL *ssl;

    // Initialize SSL
    SSL_library_init();
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL) {
        perror("SSL context creation error");
        return NULL;
    }

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        SSL_CTX_free(ctx);
        return NULL;
    }

    // Resolve domain name to IP address
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Use IPv4 or IPv6, whichever is available
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    if (getaddrinfo(server_address, portStr, &hints, &res) != 0) {
        perror("getaddrinfo");
        close(sockfd);
        SSL_CTX_free(ctx);
        return NULL;
    }

    // Attempt to connect to each resolved IP address until successful
    struct addrinfo *p;
    for (p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            perror("Socket creation error");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) != -1) {
            break; // Connection successful
        }
        close(sockfd); // Close socket if connection fails
    }
    if (p == NULL) {
        perror("Connection failed");
        freeaddrinfo(res);
        close(sockfd);
        SSL_CTX_free(ctx);
        return NULL;
    }

    // Free addrinfo structure
    freeaddrinfo(res);

    // Set up SSL connection
    ssl = SSL_new(ctx);
    if (ssl == NULL) {
        perror("SSL creation error");
        close(sockfd);
        SSL_CTX_free(ctx);
        return NULL;
    }
    SSL_set_fd(ssl, sockfd);
    if (SSL_connect(ssl) != 1) {
        perror("SSL connection error");
        close(sockfd);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return NULL;
    }

    // Free SSL context (it's no longer needed)
    SSL_CTX_free(ctx);
    
    // Construct and send the protocol header
    struct ProtocolHeader header;
    header.connectionType = SERVER_CONNECTION;
    if (SSL_write(ssl, &header, sizeof(header)) != sizeof(header)) {
        perror("Protocol header send failed");
        close(sockfd);
        SSL_free(ssl);
        return NULL;
    }

    return ssl; // Return the SSL connection object
}

void handle_server_connection(SSL *ssl) {
    // Receive the command from the other server
    const int read_size = 256;
    char buffer[read_size];
    int bytes = SSL_read(ssl, buffer, sizeof(buffer));
    buffer[bytes] = '\0';  // Ensure null termination
    
    char *token = strtok(buffer, " ");

    if(token == NULL) send_response(ssl, "INVALID_COMMAND");

    // Check if the received command is ACCOUNT_EXIST
    if (strcmp(token, "ACCOUNT_EXISTS") == 0) {
        // Receive the username from the client
        char *username = strtok(NULL, " ");
        const char *account_exists_result = account_exists(username);
        send_response(ssl, account_exists_result);
    } else {
        // Send an error response to the client indicating an invalid command
        send_response(ssl, "INVALID_COMMAND");
    }
}
