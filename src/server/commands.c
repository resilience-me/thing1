#include "commands.h"
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
    
    // Check if user directory already exists
    if (access(username, F_OK) != -1) {
        return "USERNAME_EXISTS";
    }

    // Create user directory
    if (mkdir(username, 0777) == -1) {
        return "DIRECTORY_CREATION_FAILED";
    }

    // Create and store password in password file
    char password_path[256];
    snprintf(password_path, sizeof(password_path), "%s/password", username);
    int fd = open(password_path, O_WRONLY | O_CREAT, 0600);
    if (fd == -1) {
        rmdir(username); // Remove user directory if password file creation failed
        return "PASSWORD_FILE_CREATION_FAILED";
    }
    if (write(fd, password, strlen(password)) == -1) {
        close(fd);
        unlink(password_path); // Remove password file if password write failed
        rmdir(username); // Remove user directory if password write failed
        return "PASSWORD_WRITE_FAILED";
    }
    close(fd);

    return "REGISTER_SUCCESS"; // Registration successful
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
                SSL_write(ssl, "LOGIN_SUCCESS", strlen("LOGIN_SUCCESS"));
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
