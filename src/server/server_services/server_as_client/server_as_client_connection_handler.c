const char* server_as_client_connection_handler(const char *server_address, int port, const char *command, const char *args) {
    // Establish an SSL connection to the remote server
    SSL *remoteSSL = establish_connection(server_address, port);
    if (remoteSSL == NULL) {
        return "CONNECTION_FAILED";
    }
    // Send the command to the remote server
    const char *response = server_as_client_dispatch_command(remoteSSL, command, args);

    // Close the SSL connection
    SSL_shutdown(remoteSSL);
    SSL_free(remoteSSL);
}
