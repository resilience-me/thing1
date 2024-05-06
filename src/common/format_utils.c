#include "format_utils.h"
#include "command_defs.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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

bool isNumber(const char *str) {
    // Check each character to see if it's a digit
    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i])) {
            return false;  // Non-digit character found
        }
    }
    return true;  // All characters are digits
}

// Helper function to validate port numbers
int isValidPort(const char *port) {
    if (strlen(port) == 0) return 1;  // Empty is allowed (default port)
    if (!isNumber(port)) return 0;     // Check if the string is a valid number

    int portNum = atoi(port);
    return (portNum > 0 && portNum <= 65535);
}
