#include "client_commands.h"
#include "client_connection_handler.h"
#include "server_commands.h"
#include "database.h"
#include "config.h"
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

// Function to handle user registration
const char *register_user(const char *username, const char *password) {
    // Check if username is valid
    if (!isValidUsername(username)) {
        return "INVALID_USERNAME";
    }

    // Build the path to the user directory
    char user_dir[512];
    snprintf(user_dir, sizeof(user_dir), "%s/accounts/%s", datadir, username);

    // Check if user directory already exists
    if (access(user_dir, F_OK) != -1) {
        return "USERNAME_EXISTS";
    }

    // Create user directory
    if (mkdir(user_dir, 0777) == -1) {
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

const char *delete_user(Session *session) {
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

const char *login_user(const char *username, const char *password) {
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

const char *add_connection(Session *session, char *connection_arg) {
    // Check if the user is authenticated
    if (!session->authenticated) {
        return "AUTHENTICATION_REQUIRED";
    }
    const char *server_delimiter = "@";
    const char *port_delimiter = ":";

    // Parse the connection argument
    char username[256];
    char server_address[256];
    // Set port to default value
    char port_string[6];
    snprintf(port_string, sizeof(port_string), "%d", PORT);

    char *server_username = strtok(connection_arg, server_delimiter);

    if (server_username == NULL || server_username[0] == '\0') {
        // No username specified, set an empty username
        username[0] = '\0';
    } else {
        strncpy(username, server_username, sizeof(username) - 1);
    }
    perror("1");
    // Proceed to parse server address and port
    char *server_and_port = strtok(NULL, server_delimiter);
    char *server = NULL;
    perror("2");

    if(server_and_port != NULL) {
        server = strtok(server_and_port, port_delimiter);
    }
    perror("3");

    // If server is NULL or empty, use a local connection
    if (server == NULL || server[0] == '\0') {
        strncpy(server_address, "localhost", sizeof(server_address) - 1);
    } else {
        strncpy(server_address, server, sizeof(server_address) - 1);
        char *port_str = strtok(NULL, port_delimiter);
        if (port_str != NULL) {
            strncpy(port_string, port_str, sizeof(port_string) - 1);
        }
    }
    perror("4");

    // Now 'username' contains the username, 'server_address' contains the server address,
    // and 'port' contains the port number (default if not specified)

    // Establish an SSL connection to the remote server
    SSL *remoteSSL = establish_connection(server_address, port_string);
    perror("5");

    // Check if connection was successful
    if (remoteSSL == NULL) {
        return "CONNECTION_FAILED";
    }
    perror("6");

    const char *response = send_account_exists_query(remoteSSL, username);
    perror("7");

    // Close the SSL connection
    SSL_shutdown(remoteSSL);
    SSL_free(remoteSSL);
    perror("8");

    // Check the response and take appropriate action
    if (strcmp(response, "ACCOUNT_EXISTS") == 0) {
        // Account exists, proceed with adding the connection...
        return "CONNECTION_ADDED";
    } else {
        // Account does not exist, handle error...
        return "ACCOUNT_NOT_FOUND";
    }
    
    // At this point, 'username' contains the username and 'server_address' contains the server address
    // Now you can proceed to establish the connection with the remote server and send the command
    return "PLACEHOLDER_RETURN_STRING";
}

