#include "network.h"   // Assuming it's necessary for your project's specific network functions
#include <stdio.h>     // For fprintf(), stderr
#include <stdlib.h>    // For atoi(), strndup()
#include <regex.h>     // For regex functions
#include <openssl/err.h> // If you are handling OpenSSL errors elsewhere
#define _GNU_SOURCE
#include <string.h>

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
