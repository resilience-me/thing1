#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

// Universal function to establish a socket connection
int open_socket_connection(const char *hostname, int port) {
    struct addrinfo hints, *res, *p;
    int sock;
    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;  // Support for both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;  // TCP socket

    if (getaddrinfo(hostname, port_str, &hints, &res) != 0) {
        perror("getaddrinfo failed");
        return -1;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == -1) continue;
        if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) {
            freeaddrinfo(res);
            return sock;  // Successfully connected
        }
        close(sock);
    }

    freeaddrinfo(res);
    perror("Connection failed");
    return -1;  // Indicate failure
}
