#include "thread_ssl_context.h"

static pthread_key_t client_ctx_key; // Thread-specific key for SSL context

void init_thread_ssl_context() {
    // Create a thread-specific key for the SSL context
    pthread_key_create(&client_ctx_key, NULL);
}

SSL_CTX* create_thread_ssl_context() {
    // Create a new SSL context for the current thread
    SSL_CTX *client_ctx = SSL_CTX_new(SSLv23_client_method());
    if (client_ctx == NULL) {
        // Handle error
        return NULL;
    }

    // Store the SSL context in the thread-specific data
    pthread_setspecific(client_ctx_key, client_ctx);

    return client_ctx;
}

SSL_CTX* get_thread_ssl_context() {
    // Retrieve the SSL context associated with the current thread
    return pthread_getspecific(client_ctx_key);
}
