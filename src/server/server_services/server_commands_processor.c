// #include "server_commands.h"  // Include the header file for declarations
// #include <string.h>    // For strcmp and strlen
// #include <openssl/ssl.h>  // For SSL_write

// const char* dispatch_command(SSL *ssl, const char *command, const char *args) {
//     for (int i = 0; server_commands[i].name != NULL; i++) {
//         if (strcmp(command, server_commands[i].name) == 0) {
//             return ((ServerCommandHandler) server_commands[i].handler) (session, args); // Execute the handler
//         }
//     }
// }
