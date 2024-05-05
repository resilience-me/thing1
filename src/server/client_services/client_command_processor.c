#include "client_commands.h"  // Include the header file for declarations
#include "command_defs.h"
#include <string.h>    // For strcmp and strlen
#include <openssl/ssl.h>  // For SSL_write

void dispatch_command(SSL *ssl, Session *session, const char *command, const char *args) {
    void *handler = find_command_handler(command, client_commands);
    if (handler) {
        const char *result = ((ClientCommandHandler)handler)(session, args);
        SSL_write(ssl, result, strlen(result)); // Send result back to client
    } else {
        SSL_write(ssl, "INVALID_COMMAND", strlen("INVALID_COMMAND"));
    }
}
