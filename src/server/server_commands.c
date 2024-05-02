#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

// Function to send a query to the remote server
void send_query(int sockfd, const char *query) {
    // Send the query to the server
    if (send(sockfd, query, strlen(query), 0) < 0) {
        perror("Error sending query");
    }
}

// Function to receive a response from the remote server
void receive_response(int sockfd, char *response, size_t max_length) {
    // Receive the response from the server
    ssize_t bytes_received = recv(sockfd, response, max_length - 1, 0);
    if (bytes_received < 0) {
        perror("Error receiving response");
    } else {
        // Null-terminate the response string
        response[bytes_received] = '\0';
    }
}

// Function to establish a connection to a remote server
int establish_connection(const char *server_address, int port) {
    int sockfd;
    struct sockaddr_in serv_addr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    // Set server address details
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); // Use the specified port

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, server_address, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sockfd);
        return -1;
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return -1;
    }

    return sockfd; // Return the socket file descriptor
}

const char *add_connection(Session *session, char *connection_arg) {
    // Check if the user is authenticated
    if (!session->authenticated) {
        return "AUTHENTICATION_REQUIRED";
    }

    // Parse the connection argument
    char username[256];
    char server_address[256];
    int port = PORT; // Set port to default value
    
    const char *delimiter = "@:";
    const char *server_username = strtok(connection_arg, delimiter);
    
    // If the delimiter is not found, the connection is local
    if (server_username == NULL) {
        strncpy(username, connection_arg, sizeof(username) - 1);
        strncpy(server_address, "localhost", sizeof(server_address) - 1);
    } else {
        strncpy(username, server_username, sizeof(username) - 1);
        const char *server_and_port = strtok(NULL, delimiter);
        if (server_and_port == NULL) {
            return "INVALID_CONNECTION_FORMAT";
        }
        char *server = strtok(server_and_port, ":");
        if (server == NULL) {
            return "INVALID_CONNECTION_FORMAT";
        }
        strncpy(server_address, server, sizeof(server_address) - 1);
        
        char *port_str = strtok(NULL, ":");
        if (port_str != NULL) {
            port = atoi(port_str); // Convert port string to integer
        }
    }
    
    // Now 'username' contains the username, 'server_address' contains the server address,
    // and 'port' contains the port number (default if not specified)

    // Establish a connection to the remote server
    int remote_socket = establish_connection(server_address, port);

    // Check if connection was successful
    if (remote_socket < 0) {
        return "CONNECTION_FAILED";
    }

    // Send command/query to check if the account exists
    send_query(remote_socket, username);

    // Receive response from the remote server
    char response[256];
    receive_response(remote_socket, response);

    // Close the connection
    close(remote_socket);

    // Check the response and take appropriate action
    if (strcmp(response, "ACCOUNT_EXISTS") == 0) {
        // Account exists, proceed with adding the connection...
        return "CONNECTION_ADDED";
    } else {
        // Account does not exist, handle error...
        return "ACCOUNT_NOT_FOUND";
    }
    
    // At this point, 'username' contains the username and 'server_address' contains the server address
    // Now you can proceed to establish the connection with the remote server and send the command
    return "PLACEHOLDER_RETURN_STRING";
}

void handle_server_connection(SSL *ssl) {
}
