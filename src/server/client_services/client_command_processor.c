#include "client_commands.h"  // Include the header file for declarations
#include <string.h>    // For strcmp and strlen
#include <openssl/ssl.h>  // For SSL_write

void dispatch_command(SSL *ssl, Session *session, const char *command, const char **args) {
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(command, commands[i].name) == 0) {
            const char *result;
            if (commands[i].needs_authentication && !session->authenticated) {
                result = "AUTH_REQUIRED";
            } else {
                result = commands[i].handler(session, args); // Execute the handler
            }
            SSL_write(ssl, result, strlen(result)); // Send result back to client
            return;
        }
    }
    SSL_write(ssl, "INVALID_COMMAND", strlen("INVALID_COMMAND"));
}
