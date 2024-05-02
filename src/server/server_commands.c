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
    struct ProtocolHeader header;

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
    
    // Construct and send the protocol header
    header.connectionType = SERVER_CONNECTION;
    if (send(sockfd, &header, sizeof(header), 0) != sizeof(header)) {
        perror("Protocol header send failed");
        close(sockfd);
        return -1;
    }
    
    return sockfd; // Return the socket file descriptor
}
