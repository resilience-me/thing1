#ifndef SESSION_DEFS_H
#define SESSION_DEFS_H

// As Session involves SSL types, include the SSL header
#include <openssl/ssl.h>

typedef struct {
    char username[256];       // Username for the session
    int authenticated;        // 0 = Not authenticated, 1 = Authenticated
} Session;

#endif /* SESSION_DEFS_H */
