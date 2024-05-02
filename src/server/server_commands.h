#ifndef SERVER_COMMANDS_H
#define SERVER_COMMANDS_H

// Function to send a query to the remote server
void send_query(int sockfd, const char *query);

// Function to receive a response from the remote server
void receive_response(int sockfd, char *response, size_t max_length);

// Function to establish a connection to a remote server
int establish_connection(const char *server_address, int port);

void handle_server_connection(SSL *ssl);

#endif /* SERVER_COMMANDS_H */
