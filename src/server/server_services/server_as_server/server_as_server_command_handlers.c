#include "server_as_server_command_handlers.h"
#include "server_config.h"
#include "format_utils.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h> // For snprintf
#include <unistd.h> // For access and F_OK
#include <stdio.h> // For sscanf
#include "database.h"

bool *account_exists(char *username) {
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
    
    char username[256];
    char sizeStr[256];
    
    // Parse arguments
    int parsed_args = sscanf(args, "%255s %255s %5s %255s", username, sizeStr);
    
    // Check if all arguments were successfully parsed
    if (parsed_args != 2) {
        return "INVALID_ARGUMENTS";
    }
    if(!account_exists(username)) return "FAILED_TO_SET_TRUSTLINE;
    
    
      
    // // Send an error response to the client indicating an invalid command
    // send_response(ssl, "INVALID_COMMAND");

  
  return "PLACEHOLDER RETURN STRING";
}
