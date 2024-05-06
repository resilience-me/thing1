#include "commands.h"
#include "command_handlers.h"
#include "command_defs.h"

#include <stdio.h>
#include <string.h>
#include <openssl/ssl.h>

// Array of function pointers to command handlers
void *command_handlers[] = {
    (void*)handle_login,
    (void*)handle_register,
    (void*)handle_logout,
    (void*)handle_delete_account,
    (void*)handle_add_connection,
    (void*)handle_set_trustline,
    NULL  // Include NULL for completeness
};

// Constructing the commands array
Command *commands = NULL;

void dispatch_command(SSL *ssl, const char *cmd) {
    void* handler = find_command_handler(cmd, commands);
    if (handler) {
        ((CommandHandler)handler)(ssl);  // Execute the handler assuming it matches the expected signature
    } else {
        printf("Unknown command. Please try again.\n");
    }
}
