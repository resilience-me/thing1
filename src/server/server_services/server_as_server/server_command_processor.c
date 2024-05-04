// #include "server_commands.h"  // Include the header file for declarations
// #include <string.h>    // For strcmp and strlen
// #include <openssl/ssl.h>  // For SSL_write

// void dispatch_command(SSL *ssl, const char *command, const char *args) {
//     for (int i = 0; server_as_server_commands[i].name != NULL; i++) {
//         if (strcmp(command, server_as_server_commands[i].name) == 0) {
//             const char *result = ((ServerAsServerCommandHandler)server_as_server_commands[i].handler)(session, args);
//             SSL_write(ssl, result, strlen(result)); // Send result back to client
//             return;
//         }
//     }
//     SSL_write(ssl, "INVALID_COMMAND", strlen("INVALID_COMMAND"));
// }
