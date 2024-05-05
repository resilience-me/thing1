#include "commands_util.h"  // Include the header file for declarations
#include "command_defs.h"  // Include the generic command handling definitions
#include <string.h>    // For strcmp and strlen
#include <openssl/ssl.h>  // For SSL_write

void server_as_server_dispatch_command(SSL *ssl, const char *command, const char *args) {
    void *handler = find_command_handler(command, server_as_server_commands);
    if (handler) {
        const char *result = ((ServerAsServerCommandHandler)handler)(ssl, args);
        SSL_write(ssl, result, strlen(result));  // Send result back to the other server
    } else {
        SSL_write(ssl, "INVALID_COMMAND", strlen("INVALID_COMMAND"));
    }
}
