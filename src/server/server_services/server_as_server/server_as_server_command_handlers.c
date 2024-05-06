#include "server_as_server_command_handlers.h"
#include "server_config.h"
#include "format_utils.h"
#include "network.h" 
#include <stdbool.h>
#include <string.h>
#include <stdio.h> // For snprintf
#include <unistd.h> // For access and F_OK
#include <stdio.h> // For sscanf
#include "database.h"
#include <stdlib.h>
#include <openssl/ssl.h>

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

const char *server_as_server_handle_set_trustline(SSL *ssl, const char *args) {
    char remote_username[256];
    char local_username[256];
    char sizeStr[256];
    
    // Parse arguments
    int parsed_args = sscanf(args, "%255s %255s %255s", remote_username, local_username, sizeStr);
    
    // Check if all arguments were successfully parsed
    if (parsed_args != 3) {
        printf("Error: Invalid arguments\n");
        return "INVALID_ARGUMENTS";
    }
    
    // Check if username is "none", then use default user
    if (strcmp(local_username, "none") == 0) {
        strcpy(local_username, DEFAULT_USER);  // Default username if not provided    
    }

    // Check if username is valid
    if (!isValidUsername(local_username)) {
        printf("Error: Invalid local username\n");
        return "FAILED_USERNAME_INVALID";
    }
    // Build the path to the user directory
    char user_dir[768];
    snprintf(user_dir, sizeof(user_dir), "%s/accounts/%s", datadir, local_username);

    // Check if user directory exists
    if (access(user_dir, F_OK) == -1) {
        printf("Error: Failed to access user directory\n");
        return "FAILED_TO_ACCESS_USER_DIRECTORY";
    }

    const char* remote_domain = get_domain_name(ssl);
    if (remote_domain == NULL) {
        printf("Error: Failed to get remote domain name\n");
        return "FAILED_DOMAIN_CHECK";
    }
    // Build the path to the remote user directory in the peers directory
    char peer_dir[1280];
    snprintf(peer_dir, sizeof(peer_dir), "%s/peers/%s/%s", user_dir, remote_domain, remote_username);
    
    // Create user directory
    if (make_dirs(peer_dir) == -1) {
        printf("Error: Directory creation failed\n");
        return "DIRECTORY_CREATION_FAILED";
    }
    // Construct the file path
    char filepath[1536];
    snprintf(filepath, sizeof(filepath), "%s/trustline_incoming", peer_dir);

    // Remove the file if sizeStr is "none" or 0, else update it
    if (strcmp(sizeStr, "none") == 0 || strcmp(sizeStr, "0") == 0) {
        remove(filepath);
    } else {
        // Open the file in write mode
        FILE *file = fopen(filepath, "w");
        if (file == NULL) {
            printf("Error: Failed to open file\n");
            return "FAILED_TO_OPEN_FILE";
        }
    
        // Write the trustline information to the file
        fprintf(file, "%s\n", sizeStr);
    
        // Close the file
        fclose(file);
    }
    
    // Return a success message
    return "TRUSTLINE_SET_SUCCESSFULLY";
}
