#ifndef NET_UTILS_H
#define NET_UTILS_H

// Function to open socket connection
int open_socket_connection(const char *hostname, int port);

// Function to parse URL and extract hostname and port
char* parse_url(const char *url, int *port);

#endif /* NET_UTILS_H */
