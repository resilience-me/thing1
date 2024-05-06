#ifndef SERVER_AS_SERVER_COMMAND_HANDLERS_H
#define SERVER_AS_SERVER_COMMAND_HANDLERS_H

/**
 * Handle the SET_TRUSTLINE command received from the client.
 * 
 * @param args The arguments received with the command.
 * @return A response string indicating the result of the command execution.
 */
const char *server_as_server_handle_set_trustline(SSL *ssl, const char *args);

#endif /* SERVER_AS_SERVER_COMMAND_HANDLERS_H */
