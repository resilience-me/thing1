#ifndef NETWORK_H
#define NETWORK_H

#include <openssl/ssl.h>

char* parse_url(const char *url, int *port);

#endif
