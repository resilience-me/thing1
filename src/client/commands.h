#ifndef COMMANDS_H
#define COMMANDS_H

#include <openssl/ssl.h>

void handle_login(SSL *ssl);
void handle_register(SSL *ssl);
void handle_logout(SSL *ssl);
void handle_delete_account(SSL *ssl);
void handle_add_connection(SSL *ssl);
void interact_with_server(SSL *ssl);

#endif
