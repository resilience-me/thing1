#include "format_utils.h"
#include "command_defs.h"

#include <ctype.h>

// Function to check if a username contains only valid characters
int isValidUsername(const char *username) {
    for (int i = 0; username[i] != '\0'; ++i) {
        if (!(isalnum(username[i]) || username[i] == '_')) {
            return 0; // Invalid username
        }
    }
    return 1; // Valid username
}

bool isValidArgument(const char *arg) {
    // Check if the arg contains the delimiter
    return (strchr(arg, ARGUMENT_DELIMITER) == NULL);
}
