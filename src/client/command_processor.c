#include <stdio.h>
#include <string.h>
#include <openssl/ssl.h>
#include "commands.h"
#include "command_processor.h"
#include "command_defs.h"

void dispatch_command(SSL *ssl, const char *cmd) {
    void* handler = find_command_handler(cmd, commands);
    if (handler) {
        ((CommandHandler)handler)(ssl);  // Execute the handler assuming it matches the expected signature
    } else {
        printf("Unknown command. Please try again.\n");
    }
}
