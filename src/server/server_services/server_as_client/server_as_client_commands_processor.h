#ifndef SERVER_AS_CLIENT_COMMANDS_PROCESSOR_H
#define SERVER_AS_CLIENT_COMMANDS_PROCESSOR_H

#include <openssl/ssl.h>                // For SSL type

/**
 * @brief Dispatches a command to the appropriate handler in the server as client context.
 * 
 * This function looks up the provided command in the server as client command list and dispatches it
 * to the corresponding handler function. If the command is not found, NULL is returned.
 * 
 * @param ssl SSL connection to the server.
 * @param command The command string to dispatch.
 * @param args Arguments associated with the command.
 * @return Pointer to the result string returned by the command handler, or NULL if the command is not found.
 */
const char* server_as_client_dispatch_command(SSL *ssl, const char *command, const char *args);

#endif /* SERVER_AS_CLIENT_COMMANDS_PROCESSOR_H */
