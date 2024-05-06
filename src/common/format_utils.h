#ifndef FORMAT_UTILS_H
#define FORMAT_UTILS_H

#include <stdbool.h>

bool isValidArgument(const char *arg);

// Checks if the given username contains only valid characters
int isValidUsername(const char *username);

int isValidPort(const char *port);

#endif // FORMAT_UTILS_H
