#include <stdio.h>
#include <string.h>
#include <openssl/ssl.h>
#include "commands.h"
#include "command_processor.h"
#include "command_defs.h"

void dispatch_command(SSL *ssl, const char *cmd) {
    int found = 0;

    for (Command *command = commands; command->name != NULL; command++) {
        if (strcmp(cmd, command->name) == 0) {
            if (command->handler) {  // Check if handler is not NULL
                ((CommandHandler)command->handler)(ssl);
            }
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Unknown command. Please try again.\n");
    }
}
