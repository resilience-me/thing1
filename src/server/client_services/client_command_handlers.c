#include "client_command_handlers.h"
#include "session_defs.h"
#include "network.h"
#include "commands_util.h"
#include "database.h"
#include "server_config.h"
#include "format_utils.h"

#include <ctype.h>

// Function to handle user registration
const char *client_handle_register(Session *session, const char *args) {
    char username[256] = {0}; // Ensure username is initially empty
    char password[256] = {0}; // Allow password to be empty

    if (args == NULL) {
        return "INVALID_ARGUMENTS";
    }
    sscanf(args, "%255s %255s", username, password);
    
    // Check that the username is not empty and that it is valid 
    if (username[0] == '\0' || !isValidUsername(username)) {
        return "INVALID_USERNAME";  // Username is empty or invalid
    }

    // Build the path to the user directory
    char user_dir[768];
    snprintf(user_dir, sizeof(user_dir), "%s/accounts/%s", datadir, username);

    // Check if user directory already exists
    if (access(user_dir, F_OK) != -1) {
        return "USERNAME_EXISTS";
    }

    // Create user directory
    if (make_dirs(user_dir) == -1) {
        return "DIRECTORY_CREATION_FAILED";
    }
    
    // Create and store password in password file
    char password_path[1024];
    snprintf(password_path, sizeof(password_path), "%s/password", user_dir);
    int fd = open(password_path, O_WRONLY | O_CREAT, 0600);
    if (fd == -1) {
        rmdir(user_dir); // Remove user directory if password file creation failed
        return "PASSWORD_FILE_CREATION_FAILED";
    }
    if (write(fd, password, strlen(password)) == -1) {
        close(fd);
        unlink(password_path); // Remove password file if password write failed
        rmdir(user_dir); // Remove user directory if password write failed
        return "PASSWORD_WRITE_FAILED";
    }
    close(fd);

    return "REGISTER_SUCCESS"; // Registration successful
}

const char *client_handle_delete_account(Session *session, const char *args) {
    if (!session->authenticated) {
        return "AUTH_REQUIRED";
    }

    char user_dir[1024];
    snprintf(user_dir, sizeof(user_dir), "%s/accounts/%s", datadir, session->username);

    // Delete the user directory and all its contents securely
    char command[2048];
    snprintf(command, sizeof(command), "rm -rf %s", user_dir);
    int status = system(command);
    if (status != 0) {
        return "FAILED_TO_DELETE_USER";
    }

    // Successfully deleted the user, now unauthenticate the session
    memset(session->username, 0, sizeof(session->username)); // Clear the username
    session->authenticated = 0; // Unauthenticate the session

    return "USER_DELETED_SUCCESSFULLY";
}

const char *client_handle_login(Session *session, const char *args) {
    char username[256] = {0}; // Ensure username is initially empty
    char password[256] = {0}; // Allow password to be empty

    if (args == NULL) {
        return "INVALID_ARGUMENTS";
    }
    sscanf(args, "%255s %255s", username, password);
    
    // Check that the username is not empty and that it is valid 
    if (username[0] == '\0' || !isValidUsername(username)) {
        return "INVALID_USERNAME";  // Username is empty or invalid
    }

    // Build the path to the user directory
    char user_dir[768];
    snprintf(user_dir, sizeof(user_dir), "%s/accounts/%s", datadir, username);

    // Check if user directory exists
    if (access(user_dir, F_OK) == -1) {
        return "USERNAME_NOT_FOUND";
    }

    // Build the path to the password file
    char password_path[1024];
    snprintf(password_path, sizeof(password_path), "%s/password", user_dir);

    // Open and read the stored password
    int fd = open(password_path, O_RDONLY);
    if (fd == -1) {
        return "PASSWORD_FILE_NOT_FOUND";
    }

    char stored_password[256];
    int read_bytes = read(fd, stored_password, sizeof(stored_password) - 1);
    if (read_bytes == -1) {
        close(fd);
        return "PASSWORD_READ_FAILED";
    }
    
    // Remove the newline character if it exists
    if (read_bytes > 0 && stored_password[read_bytes - 1] == '\n') {
        stored_password[read_bytes - 1] = '\0';
    }
    
    stored_password[read_bytes] = '\0';  // Null terminate the password read from file
    close(fd);
    
    // Compare the provided password with the stored password
    if (strcmp(password, stored_password) != 0) {
        return "PASSWORD_INCORRECT";
    }

    strncpy(session->username, username, sizeof(session->username)-1);
    session->authenticated = 1;
    
    return "LOGIN_SUCCESS";  // User successfully authenticated
}

const char *client_handle_logout(Session *session, const char *args) {
    session->authenticated = 0;
    return "LOGOUT_SUCCESS";
}


const char *add_account(const char *username, const char *server_address, const char *portStr, Session *session) {

    // Build the path to the user directory within the port directory
    char user_dir[1280];
    snprintf(user_dir, sizeof(user_dir), "%s/accounts/%s/peers/%s/%s/%s", datadir, session->username, server_address, portStr, username);

    // Check if user directory already exists
    if (access(user_dir, F_OK) != -1) {
        printf("Account already connected\n");
        return "ALREADY_CONNECTED";
    }

    // Consolidated directory creation using make_dirs
    if (make_dirs(user_dir) == -1) {
        printf("Failed to add account\n");
        return "DIRECTORY_CREATION_FAILED";
    }

    printf("Account added successfully\n");
    return "CONNECTION_ADDED";
}


const char *client_handle_add_connection(Session *session, const char *args) {
    char remote_username[256] = "";
    char server_address[256] = "";
    char portStr[6] = "";

    // Check authentication
    if (!session->authenticated) {
        return "AUTH_REQUIRED";
    }

    // Parse arguments
    sscanf(args, "%255s %255s %5s", remote_username, server_address, portStr);

    // Set defaults for empty inputs
    if (remote_username[0] == '\0') {
        strcpy(remote_username, "default");  // Default username if not provided
    }
    
    // Check if provided username is invalid
    if (!isValidUsername(remote_username)) {
        return "INVALID_USERNAME";
    }

    // Validate server address
    if (server_address[0] == '\0' || strcmp(server_address, SERVER_ADDRESS) == 0) {
        strcpy(server_address, "localhost");  // Default to localhost if no server address is provided
    }

    char port_buf[6]; // Assuming port number will be less than 100000
    
    // If portStr is NULL or empty, set it to DEFAULT_PORT
    if (portStr[0] == '\0') {
        sprintf(port_buf, "%d", SERVER_DEFAULT_PORT);
        strcpy(portStr, port_buf);
    }
    
    // Validate port
    int port = atoi(portStr);  // Convert string to int
    if (port <= 0) {       // Simple validation to catch invalid conversions
        return "INVALID_PORT";
    }

    // Establish an SSL connection to the remote server
    SSL *remoteSSL = establish_connection(server_address, port);
    if (remoteSSL == NULL) {
        return "CONNECTION_FAILED";
    }

    // Send a query to check if the account exists on the remote server
    const char *response = server_as_client_dispatch_command(remoteSSL, "ACCOUNT_EXISTS", remote_username);
    
    // Handle the response accordingly
    if (response) {
        // Handle the response, such as checking if the account exists
        printf("Response from server: %s\n", response);
    } else {
        // Handle the case where the command handler was not found or returned NULL
        printf("Failed to execute command: ACCOUNT_EXISTS\n");
    }

    // Close the SSL connection
    SSL_shutdown(remoteSSL);
    SSL_free(remoteSSL);

    // Handle the response from the server
    if (strcmp(response, "ACCOUNT_EXISTS") == 0) {
        // Prevent adding self as a connection
        if (strcmp(server_address, "localhost") == 0 && strcmp(remote_username, session->username) == 0) {
            return "CANNOT_ADD_SELF";
        }
        return add_account(remote_username, server_address, portStr, session);
    } else {
        return "ACCOUNT_NOT_FOUND";
    }
}

const char *client_handle_set_trustline(Session *session, const char *args) {
    char remote_username[256];
    char server_address[256];
    char portStr[6];
    char sizeStr[256];

    // Check authentication
    if (!session->authenticated) {
        return "AUTH_REQUIRED";
    }

    // Parse arguments
    int parsed_args = sscanf(args, "%255s %255s %5s %255s", remote_username, server_address, portStr, sizeStr);
    
    // Check if all arguments were successfully parsed
    if (parsed_args != 4) {
        return "INVALID_ARGUMENTS";
    }

    // Check if provided username is invalid
    if (!isValidUsername(remote_username)) {
        return "INVALID_USERNAME";
    }
    // Set address to localhost if arg emptt
    if(strcmp(server_address, "none") == 0) {
        strcpy(server_address, "localhost");  // Default to localhost if no server address is provided
    }

    char port_buf[6]; // Assuming port number will be less than 100000

    if(strcmp(portStr, "none") == 0) {
        sprintf(port_buf, "%d", SERVER_DEFAULT_PORT);
        strcpy(portStr, port_buf);
    }

    // Validate port
    int port = atoi(portStr);  // Convert string to int
    if (port <= 0) {       // Simple validation to catch invalid conversions
        return "INVALID_PORT";
    }

    // Handle size - set to "0" if empty
    if (sizeStr[0] == '\0') {
        strcpy(sizeStr, "0");  // Default size to "0" if no size was provided
    }    
    
    // Establish an SSL connection to the remote server
    SSL *remoteSSL = establish_connection(server_address, port);
    if (remoteSSL == NULL) {
        return "CONNECTION_FAILED";
    }

    // Send the command to the remote server
    const char *response = server_as_client_dispatch_command(remoteSSL, "SET_TRUSTLINE", sizeStr);

    // Close the SSL connection
    SSL_shutdown(remoteSSL);
    SSL_free(remoteSSL);
    if(response) {
        // Handle the response from the server
        if (strcmp(response, "SUCCESS") == 0) {
            return "add_account(remote_username, server_address, portStr, session)";
        } else {
            return "ACCOUNT_NOT_FOUND";
        }
    } else {
        return "FAILED_TO_SET_TRUSTLINE";
    }
}
