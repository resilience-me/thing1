#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include <netdb.h>
#include <regex.h>

void configure_context(SSL_CTX *ctx) {
    SSL_CTX_set_default_verify_paths(ctx);
}

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
        if (pmatch[3].rm_so != -1) {  // Port is specified
            *port = atoi(url + pmatch[3].rm_so);
        }
    } else {
        fprintf(stderr, "URL format error\n");
    }

    regfree(&regex);
    return hostname;
}
