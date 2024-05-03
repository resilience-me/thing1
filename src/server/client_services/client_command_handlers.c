#include "client_command_handlers.h"
#include "session_defs.h"
#include "network.h"
#include "server_commands.h"
#include "database.h"
#include "server_config.h"
#include "format_utils.h"

#include <ctype.h>

// Function to handle user registration
const char *register_user(Session *session, const char *args) {
    char username[256];
    char password[256];
    if (sscanf(args, "%255s %255s", username, password) != 2) {
        return "INVALID_ARGUMENTS";
    }
    // Check if username is valid
    if (!isValidUsername(username)) {
        return "INVALID_USERNAME";
    }

    // Build the path to the user directory
    char user_dir[768];
    snprintf(user_dir, sizeof(user_dir), "%s/accounts/%s", datadir, username);

    // Check if user directory already exists
    if (access(user_dir, F_OK) != -1) {
        return "USERNAME_EXISTS";
    }

    // Create user directory
    if (mkdir(user_dir, 0777) == -1) {
        return "DIRECTORY_CREATION_FAILED";
    }

    // Create peers directory within the user directory
    char peers_dir[1024];
    snprintf(peers_dir, sizeof(peers_dir), "%s/peers", user_dir);
    if (mkdir(peers_dir, 0777) == -1) {
        return "PEERS_DIRECTORY_CREATION_FAILED";
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

const char *delete_user(Session *session, const char *args) {
    if (!session->authenticated) {
        return "AUTHENTICATION_REQUIRED";
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

const char *login_user(Session *session, const char *args) {
    // Check if username is valid
    if (!isValidUsername(username)) {
        return "INVALID_USERNAME";
    }

    // Build the path to the user directory
    char user_dir[512];
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

    stored_password[read_bytes] = '\0';  // Null terminate the password read from file
    close(fd);

    // Compare the provided password with the stored password
    if (strcmp(password, stored_password) != 0) {
        return "PASSWORD_INCORRECT";
    }

    return "LOGIN_SUCCESS";  // User successfully authenticated
}

const char *logout_user(Session *session, const char *args) {
    session.authenticated = 0;
}


const char *add_account(const char *username, const char *server_address, const char *portStr, Session *session) {
    // Build the path to the server directory
    char server_dir[768];
    snprintf(server_dir, sizeof(server_dir), "%s/accounts/%s/peers/%s", datadir, session->username, server_address);
    
    // Build the path to the port directory within the server directory
    char port_dir[1024];
    snprintf(port_dir, sizeof(port_dir), "%s/%s", server_dir, portStr);

    // Build the path to the user directory within the port directory
    char user_dir[1280];
    snprintf(user_dir, sizeof(user_dir), "%s/%s", port_dir, username);

    // Check if user directory already exists
    if (access(user_dir, F_OK) != -1) {
        printf("Account already connected\n");
        return "ALREADY_CONNECTED";
    }

    // Create server directory if it doesn't exist
    if (access(server_dir, F_OK) == -1) {
        if (mkdir(server_dir, 0777) == -1) {
            printf("Failed to add account\n");
            return "DIRECTORY_CREATION_FAILED";
        }
    }

    // Create port directory if it doesn't exist
    if (access(port_dir, F_OK) == -1) {
        if (mkdir(port_dir, 0777) == -1) {
            printf("Failed to add account\n");
            return "DIRECTORY_CREATION_FAILED";
        }
    }

    // Create user directory
    if (mkdir(user_dir, 0777) == -1) {
        printf("Failed to add account\n");
        return "DIRECTORY_CREATION_FAILED";
    }

    printf("Account added successfully\n");
    return "CONNECTION_ADDED";
}


const char *add_connection(Session *session, const char *args) {
    // Check if the user is authenticated
    if (!session->authenticated) {
        return "AUTHENTICATION_REQUIRED";
    }

    // Set empty string if username is NULL
    if (username == NULL) {
        username = "";  // Empty string
    }

    // Check if provided username is invalid
    if (!isValidUsername(username)) {
        return "INVALID_USERNAME";
    }

    // Validate server address
    if (server_address == NULL || server_address[0] == '\0') {
        server_address = "localhost";  // Default to localhost if no server address is provided
    }

    char port_buf[6]; // Assuming port number will be less than 100000
    
    // If portStr is NULL or empty, set it to DEFAULT_PORT
    if (portStr == NULL || portStr[0] == '\0') {
        sprintf(port_buf, "%d", SERVER_DEFAULT_PORT);
        portStr = port_buf;
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
    const char *response = send_account_exists_query(remoteSSL, username);

    // Close the SSL connection
    SSL_shutdown(remoteSSL);
    SSL_free(remoteSSL);

    // Handle the response from the server
    if (strcmp(response, "ACCOUNT_EXISTS") == 0) {

        // Set username to "default"´ if username is empty string
        if (strcmp(username, "") == 0) {
            username = "default";  // Set to "default"
        }
        if (strcmp(server_address, "localhost") == 0 && strcmp(username, session->username) == 0) return "CANNOT_ADD_SELF";
        
        return add_account(username, server_address, portStr, session);
    } else {
        return "ACCOUNT_NOT_FOUND";
    }
}


