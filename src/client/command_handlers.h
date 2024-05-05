#ifndef COMMAND_HANDLERS_H
#define COMMAND_HANDLERS_H

#include <openssl/ssl.h>

// Function declarations
void handle_login(SSL *ssl);
void handle_register(SSL *ssl);
void handle_logout(SSL *ssl);
void handle_delete_account(SSL *ssl);
void handle_add_connection(SSL *ssl);
void handle_set_trustline(SSL *ssl);

#endif // COMMAND_HANDLERS_H
