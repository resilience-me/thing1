#include "commands.h"
#include "database.h"
#include "config.h"
#include <ctype.h>

// Constants for connection types
#define CLIENT_CONNECTION 0x01
#define SERVER_CONNECTION 0x02

struct ProtocolHeader {
    uint8_t connectionType; // Identifier field indicating connection type
    // Other fields...
};

typedef struct {
    char username[256];   // Username for the session
    int authenticated;    // 0 = Not authenticated, 1 = Authenticated
} Session;

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

// Function to establish a connection to a remote server
int establish_connection(const char *server_address) {
    int sockfd;
    struct sockaddr_in serv_addr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    // Set server address details
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(REMOTE_SERVER_PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, server_address, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sockfd);
        return -1;
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return -1;
    }

    return sockfd; // Return the socket file descriptor
}

const char *add_connection(Session *session, char *connection_arg) {
    // Check if the user is authenticated
    if (!session->authenticated) {
        return "AUTHENTICATION_REQUIRED";
    }

    // Parse the connection argument
    char username[256];
    char server_address[256];
    int port = PORT; // Set port to default value
    
    const char *delimiter = "@:";
    const char *server_username = strtok(connection_arg, delimiter);
    
    // If the delimiter is not found, the connection is local
    if (server_username == NULL) {
        strncpy(username, connection_arg, sizeof(username) - 1);
        strncpy(server_address, "localhost", sizeof(server_address) - 1);
    } else {
        strncpy(username, server_username, sizeof(username) - 1);
        const char *server_and_port = strtok(NULL, delimiter);
        if (server_and_port == NULL) {
            return "INVALID_CONNECTION_FORMAT";
        }
        char *server = strtok(server_and_port, ":");
        if (server == NULL) {
            return "INVALID_CONNECTION_FORMAT";
        }
        strncpy(server_address, server, sizeof(server_address) - 1);
        
        char *port_str = strtok(NULL, ":");
        if (port_str != NULL) {
            port = atoi(port_str); // Convert port string to integer
        }
    }
    
    // Now 'username' contains the username, 'server_address' contains the server address,
    // and 'port' contains the port number (default if not specified)

    // Establish a connection to the remote server
    int remote_socket = establish_connection(server_address);

    // Check if connection was successful
    if (remote_socket < 0) {
        return "CONNECTION_FAILED";
    }

    // Send command/query to check if the account exists
    send_query(remote_socket, username);

    // Receive response from the remote server
    char response[256];
    receive_response(remote_socket, response);

    // Close the connection
    close(remote_socket);

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

void handle_client_connection(SSL *ssl) {

    // Send acknowledgment to the client
    const char *ack_message = "Client connection established";
    SSL_write(ssl, ack_message, strlen(ack_message));

    Session session = {0};  // Initializes username and authenticated status
    
    const int read_size = 256;
    char buffer[read_size];
    int bytes;

    while (1) {
        bytes = SSL_read(ssl, buffer, sizeof(buffer));
        if (bytes <= 0) break;  // No more data or error occurred
        buffer[bytes] = '\0';  // Ensure null termination

        char *token = strtok(buffer, " ");
        if (token == NULL) continue;  // Skip empty input

        if (!session.authenticated) {
            if (strcmp(token, "LOGIN") == 0) {
                char *username = strtok(NULL, " ");
                char *password = strtok(NULL, " ");
                if (username && password) {
                    const char *login_result = login_user(username, password);
                    if (strcmp(login_result, "LOGIN_SUCCESS") == 0) {
                        strncpy(session.username, username, sizeof(session.username)-1);
                        session.authenticated = 1;
                    }
                    SSL_write(ssl, login_result, strlen(login_result));
                } else {
                    SSL_write(ssl, "INVALID_LOGIN_COMMAND", strlen("INVALID_LOGIN_COMMAND"));
                }
            } else if (strcmp(token, "REGISTER") == 0) {
                char *username = strtok(NULL, " ");
                char *password = strtok(NULL, " ");
                if (username && password) {
                    const char *register_result = register_user(username, password);
                    SSL_write(ssl, register_result, strlen(register_result));
                } else {
                    SSL_write(ssl, "INVALID_REGISTER_COMMAND", strlen("INVALID_REGISTER_COMMAND"));
                }
            } else {
                SSL_write(ssl, "AUTH_REQUIRED", strlen("AUTH_REQUIRED"));
            }
        } else {
            // Process commands for authenticated users
            if (strcmp(token, "LOGOUT") == 0) {
                session.authenticated = 0;
                SSL_write(ssl, "LOGOUT_SUCCESS", strlen("LOGOUT_SUCCESS"));
            } else if (strcmp(token, "DELETE_ACCOUNT") == 0) {
                const char *delete_result = delete_user(&session);
                SSL_write(ssl, delete_result, strlen(delete_result));
            } else if (strcmp(token, "ADD_CONNECTION") == 0) {
                char *remote_username = strtok(NULL, " ");
                if (remote_username) {
                    // Establish connection with remote server and send ADD_CONNECTION command
                    const char *add_connection_result = add_connection(&session, remote_username);
                    SSL_write(ssl, add_connection_result, strlen(add_connection_result));
                } else {
                    SSL_write(ssl, "INVALID_ADD_CONNECTION_COMMAND", strlen("INVALID_ADD_CONNECTION_COMMAND"));
                }
            } else {
                SSL_write(ssl, "INVALID_COMMAND", strlen("INVALID_COMMAND"));  // Could be further elaborated to handle specific commands
            }
        }
    }
}
void handle_server_connection(SSL *ssl) {
}

void *handle_connection(void *arg) {
    SSL *ssl = (SSL *)arg;
    
    // Perform SSL handshake
    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        goto cleanup;
    }
    
    struct ProtocolHeader header;
    SSL_read(ssl, &header, sizeof(header));

    // Determine connection type based on the identifier
    if (header.connectionType == CLIENT_CONNECTION) {
        handle_client_connection(ssl);
    } else if (header.connectionType == SERVER_CONNECTION) {
        handle_server_connection(ssl);
    } else {
        // Invalid connection type
    }

cleanup:
    SSL_free(ssl);
    pthread_exit(NULL);
}
