#include "net_utils.h"
#include <stdio.h>      // For perror
#include <stdlib.h>     // For exit
#include <string.h>     // For memset, snprintf
#include <unistd.h> // For close()

// Function to resolve the hostname and port
int resolve_hostname(const char *hostname, const char *port_str, struct addrinfo **res) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, port_str, &hints, res);
    if (status != 0) {
        perror("getaddrinfo failed");
        return -1;
    }
    return 0;
}

// Function to create a socket and attempt a connection
int create_and_connect_socket(struct addrinfo *res) {
    struct addrinfo *p;
    int sock;

    for (p = res; p != NULL; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == -1) continue;
        if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) return sock;
        close(sock);
    }

    return -1;
}

int open_connection(const char *hostname, int port) {
    struct addrinfo *res;
    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port);

    if (resolve_hostname(hostname, port_str, &res) != 0) {
        return -1;  // Return -1 on failure
    }

    int sock = create_and_connect_socket(res);
    freeaddrinfo(res);

    if (sock == -1) {
        return -1;  // Return -1 on failure
    }

    return sock;  // Return socket file descriptor
}
