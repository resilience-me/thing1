// #include "server_commands.h"  // Include the header file for declarations
// #include <string.h>    // For strcmp and strlen
// #include <openssl/ssl.h>  // For SSL_write

// const char* dispatch_command(SSL *ssl, const char *command, const char *args) {
//     for (int i = 0; server_as_client_commands[i].name != NULL; i++) {
//         if (strcmp(command, server_as_client_commands[i].name) == 0) {
//             return ((ServerAsClientCommandHandler) server_as_client_commands[i].handler) (ssl, args); // Execute the handler
//         }
//     }
// }
