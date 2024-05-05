#include "server_as_client_commands_processor.h"
#include "server_commands.h"  // Include the header file for declarations
#include "command_defs.h"  // Include the generic command handling definitions
#include "server_as_client_commands.h"
#include <string.h>    // For strcmp and strlen
#include <openssl/ssl.h>  // For SSL_write

const char* server_as_client_dispatch_command(SSL *ssl, const char *command, const char *args) {
    void *handler = find_command_handler(command, server_as_client_commands);
    if (handler) {
        return ((ServerAsClientCommandHandler)handler)(ssl, args);  // Execute the handler, assuming it exists
    }
    return NULL;  // Return NULL or some appropriate error handling code
}
