#ifndef SERVER_COMMANDS_H
#define SERVER_COMMANDS_H

#include <openssl/ssl.h> // Include OpenSSL header

// Function to send a query to the remote server using SSL
void ssl_send_query(SSL *ssl, const char *query);

// Function to receive a response from the remote server using SSL
void ssl_receive_response(SSL *ssl, char *response, size_t max_length);

// Function to establish an SSL connection to a remote server
SSL* ssl_establish_connection(const char *server_address, int port);

#endif /* SERVER_COMMANDS_H */
