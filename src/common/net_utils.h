// net_utils.h
#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// Function to resolve the hostname and port
int resolve_hostname(const char *hostname, const char *port_str, struct addrinfo **res);

// Function to create a socket and attempt a connection
int create_and_connect_socket(struct addrinfo *res);

// Function to open a TCP connection to a specified hostname and port
int open_connection(const char *hostname, int port);

#endif // NET_UTILS_H
