#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <regex.h>

// Universal function to establish a socket connection
int open_socket_connection(const char *hostname, int port) {
    struct addrinfo hints, *res, *p;
    int sock;
    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, port_str, &hints, &res) != 0) {
        perror("getaddrinfo failed");
        exit(EXIT_FAILURE);
    }

    for (p = res; p != NULL; p = p’s next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == -1) continue;
        if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) break;
        close(sock);
    }

    freeaddrinfo(res);
    if (p == NULL) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    return sock;
}

// Function to parse URLs into hostname and port
char* parse_url(const char *url, int *port) {
    regex_t regex;
    regmatch_t pmatch[4];
    const char *pattern = "^([^:/]+)(:([0-9]+))?/?$";
    char *hostname = NULL;

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        fprintf(stderr, "Could not compile regex\n");
        return NULL;
    }

    if (regexec(&regex, url, 4, pmatch, 0) == 0) {
        hostname = strndup(url + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
        if (pmatch[3].rm_so != -1) {
            *port = atoi(url + pmatch[3].rm_so);
        }
    } else {
        fprintf(stderr, "URL format error\n");
    }

    regfree(&regex);
    return hostname;
}
