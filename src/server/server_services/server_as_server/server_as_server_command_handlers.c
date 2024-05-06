#include "server_as_server_command_handlers.h"
#include "server_config.h"
#include "format_utils.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h> // For snprintf
#include <unistd.h> // For access and F_OK
#include <stdio.h> // For sscanf
#include "database.h"
#include <stdlib.h>

bool account_exists(char *username) {
    // Check if username is "none", then use default user
    if (strcmp(username, "none") == 0) {
          username = DEFAULT_USER;
    }

    // Check if username is valid
    if (!isValidUsername(username)) {
        return false;
    }
    
    // Build the path to the user directory
    char user_dir[512];
    snprintf(user_dir, sizeof(user_dir), "%s/accounts/%s", datadir, username);

    // Check if user directory exists
    if (access(user_dir, F_OK) == -1) {
        return false;
    }
    // Return NULL or success indicator
    return true;
}

const char *server_as_server_handle_set_trustline(const char *args) {
    char remote_username[256];
    char local_username[256];
    char sizeStr[256];
    
    // Parse arguments
    int parsed_args = sscanf(args, "%255s %255s %255s", remote_username, local_username, sizeStr);
    
    // Check if all arguments were successfully parsed
    if (parsed_args != 3) {
        return "INVALID_ARGUMENTS";
    }

    // Check if username is "none", then use default user
    if (strcmp(local_username, "none") == 0) {
          username = DEFAULT_USER;
    }

    // Check if username is valid
    if (!isValidUsername(local_username)) {
        return false;
    }
    // Build the path to the user directory
    char user_dir[768];
    snprintf(user_dir, sizeof(user_dir), "%s/accounts/%s", datadir, local_username);

    // Check if user directory exists
    if (access(user_dir, F_OK) == -1) {
        return false;
    }

    // Construct the file path
    char filepath[1280];
    snprintf(filepath, sizeof(filepath), "%s/peers/%s/trustline_incoming", user_dir, remote_username);

    // Open the file in write mode
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        return "FAILED_TO_OPEN_FILE";
    }

    // Write the trustline information to the file
    fprintf(file, "%s\n", sizeStr);

    // Close the file
    fclose(file);

    // Return a success message
    return "TRUSTLINE_SET_SUCCESSFULLY";
}
