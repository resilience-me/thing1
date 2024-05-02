#ifndef SERVER_COMMANDS_H
#define SERVER_COMMANDS_H

#include <openssl/ssl.h> // Include OpenSSL header

// Function to send a query to the remote server using SSL
void send_query(SSL *ssl, const char *query);

// Function to receive a response from the remote server using SSL
void receive_response(SSL *ssl, char *response, size_t max_length);

const char *account_exists(char *username);

const char *send_account_exists_query(SSL *ssl, const char *username);

// Function to establish an SSL connection to a remote server
SSL* establish_connection(const char *server_address, char *portStr);

#endif /* SERVER_COMMANDS_H */
