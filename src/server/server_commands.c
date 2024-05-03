#include "connection_handler.h"
#include "client_command_handlers.h"
#include "database.h"
#include "server_config.h"
#include "format_utils.h"

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
