#ifndef PARSE_STRING_UTIL_H
#define PARSE_STRING_UTIL_H

// Checks if the given username contains only valid characters
int isValidUsername(const char *username);

// Other string-related utility functions can be declared here
char* parse_url(const char *url, int *port);

#endif // PARSE_STRING_UTIL_H
