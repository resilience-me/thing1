#include "format_utils.h"
#include "command_defs.h"

#include <stdlib.h>
#include <ctype.h>

bool isValidArgument(const char *arg) {
    // Check if the arg contains the delimiter
    return (strchr(arg, ARGUMENT_DELIMITER) == NULL);
}

// Function to check if a username contains only valid characters
int isValidUsername(const char *username) {
    for (int i = 0; username[i] != '\0'; ++i) {
        if (!(isalnum(username[i]) || username[i] == '_')) {
            return 0; // Invalid username
        }
    }
    return 1; // Valid username
}

// Helper function to validate port numbers
int isValidPort(const char *port) {
    if (strlen(port) == 0) return 1;  // Empty is allowed (default port)
    for (int i = 0; port[i]; i++) {
        if (!isdigit(port[i])) {
            return 0;  // Non-digit character found
        }
    }
    int portNum = atoi(port);
    return (portNum > 0 && portNum <= 65535);
}
