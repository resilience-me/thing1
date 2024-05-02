#include "connection_handler.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <openssl/ssl.h>
#include <netdb.h> // Add this line for addrinfo

// Function to send a query to the remote server
void send_query(SSL *ssl, const char *query) {
    // Send the query to the server
    if (SSL_write(ssl, query, strlen(query)) < 0) {
        perror("Error sending query");
    }
}

// Function to receive a response from the remote server
void receive_response(SSL *ssl, char *response, size_t max_length) {
    // Receive the response from the server
    ssize_t bytes_received = SSL_read(ssl, response, max_length - 1);
    if (bytes_received < 0) {
        perror("Error receiving response");
    } else {
        // Null-terminate the response string
        response[bytes_received] = '\0';
    }
}

const char *account_exists(char *username) {
    // Check if username is NULL or empty, then use default user
    if (username == NULL || username[0] == '\0') {
        username = DEFAULT_USER;
    }

    // Check if username is valid
    if (!isValidUsername(username)) {
        return "INVALID_USERNAME";
    }
    
    // Build the path to the user directory
    char user_dir[512];
    snprintf(user_dir, sizeof(user_dir), "%s/accounts/%s", datadir, username);

    // Check if user directory exists
    if (access(user_dir, F_OK) == -1) {
        return "USERNAME_NOT_FOUND";
    }

    // Return NULL or success indicator
    return "ACCOUNT_EXISTS";
}


const char *send_account_exists_query(SSL *ssl, const char *username) {
    // Construct the query with the username
    char query[256];
    snprintf(query, sizeof(query), "ACCOUNT_EXISTS %s", username);

    // Send the query to the server
    send_query(ssl, query);

    // Receive the response from the server
    char response[256];
    receive_response(ssl, response, sizeof(response));
    
    // Return the server response
    return strdup(response); // Remember to free the memory allocated by strdup
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
