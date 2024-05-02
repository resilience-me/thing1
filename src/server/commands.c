#include "commands.h"
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

void *handle_connection(void *arg) {
    SSL *ssl = (SSL *)arg;
    const int read_size = 256;
    char buffer[read_size];
    int bytes;

    // Perform SSL handshake
    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        goto cleanup;
    }

    // Receive and process commands from the client
    while (1) {
        bytes = SSL_read(ssl, buffer, sizeof(buffer));
        if (bytes <= 0) {
            break; // No more data or error occurred
        }
        buffer[bytes] = '\0';
        printf("Received: %s\n", buffer);

        // Parse the received command
        char *token = strtok(buffer, " ");
        if (token != NULL) {
            printf("Received token: %s\n", token);

            if (strcmp(token, "LOGIN") == 0) {
                // Extract username and password from the command
                char *username = strtok(NULL, " ");
                char *password = strtok(NULL, " ");
                if (username == NULL || password == NULL) {
                    SSL_write(ssl, "INVALID_LOGIN_COMMAND", strlen("INVALID_LOGIN_COMMAND"));
                    continue;
                }
            
                const char *result = login_user(username, password);
                SSL_write(ssl, result, strlen(result));
            } else if (strcmp(token, "REGISTER") == 0) {
                // Extract username and password from the command
                char *username = strtok(NULL, " ");
                char *password = strtok(NULL, " ");
                
                if (username == NULL || password == NULL) {
                    SSL_write(ssl, "INVALID_REGISTER_COMMAND", strlen("INVALID_REGISTER_COMMAND"));
                    continue;
                }

                const char *result = register_user(username, password);
                SSL_write(ssl, result, strlen(result));
            } else {
                SSL_write(ssl, "INVALID_COMMAND", strlen("INVALID_COMMAND"));
            }
        }
    }

cleanup:
    SSL_free(ssl);
    pthread_exit(NULL);
}
