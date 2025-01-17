#include "network.h"        // Include corresponding header first

#include "server_config.h"  // Configuration settings specific to the server
#include "net_utils.h"      // Network utility functions
#include "ssl_utils.h"      // SSL utility functions
#include "protocol_defs.h"  // Protocol header to distinguish betwween server and client connections

#include <openssl/ssl.h>
#include <openssl/x509.h>

SSL_CTX *create_ssl_server_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_ecdh_auto(ctx, 1);

    return ctx;
}

void configure_ssl_server_context(SSL_CTX *ctx) {
    if (SSL_CTX_use_certificate_chain_file(ctx, CERTIFICATE_FILE) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    if (SSL_CTX_use_PrivateKey_file(ctx, PRIVATE_KEY_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Private key does not match the public certificate\n");
        exit(EXIT_FAILURE);
    }
    SSL_CTX_set_default_verify_paths(ctx);
}

int create_socket(int port) {
    int s;
    struct sockaddr_in addr;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    // Set SO_REUSEADDR option
    int optval = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("Unable to set SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(s, 5) < 0) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }

    return s;
}

// Function to establish a connection to a remote server
SSL* establish_connection(const char *server_address, int port) {
    init_openssl();

    SSL_CTX *ctx = global_client_ctx; // Use global client context
    if (ctx == NULL) {
        fprintf(stderr, "Global client SSL context not initialized\n");
        return NULL;
    }

    int sockfd = open_connection(server_address, port);
    if (sockfd == -1) {
        return NULL;
    }

    // Send protocol header
    struct ProtocolHeader header;
    header.connectionType = SERVER_CONNECTION;
    if (send(sockfd, &header, sizeof(header), 0) != sizeof(header)) {
        perror("Failed to send protocol header");
        close(sockfd);
        return NULL;
    }

    SSL *ssl = setup_ssl_client_connection(ctx, sockfd);
    if (ssl == NULL) {
        close(sockfd);
        return NULL;
    }

    // Optionally, perform post-handshake checks or configurations here
    printf("SSL Handshake successful with %s encryption\n", SSL_get_cipher(ssl));

    return ssl;
}

const char *get_domain_name(SSL *ssl) {
    X509_NAME *subject_name = get_peer_subject_name(ssl);
    if (!subject_name) {
        printf("Failed to get subject name\n");
        return NULL;  // Handle failure to get subject name
    }

    const char *peer_ip = get_peer_ip(subject_name);
    if (!peer_ip) {
        printf("Failed to get peer IP\n");
        X509_NAME_free(subject_name);
        return NULL;  // Handle failure to get peer IP
    }

    if (is_localhost(peer_ip)) {
        printf("Peer IP is localhost\n");
        X509_NAME_free(subject_name);
        return "localhost";
    }

    const char *common_name = get_peer_certificate_common_name(subject_name);
    if (!common_name) {
        printf("Failed to get common name\n");
        X509_NAME_free(subject_name); // Free subject_name before returning NULL
        return NULL; // Handle failure to get common name
    }

    // Make a copy of the common name before freeing subject_name
    char *domain_name = strdup(common_name);
    if (!domain_name) {
        printf("Failed to allocate memory for domain name\n");
        X509_NAME_free(subject_name);
        return NULL; // Handle memory allocation failure
    }

    X509_NAME_free(subject_name);
    return domain_name;
}


X509_NAME *get_peer_subject_name(SSL *ssl) {
    X509 *peer_cert = SSL_get_peer_certificate(ssl);
    if (!peer_cert) {
        printf("No certificate available\n");
        return NULL;  // No certificate available
    }

    X509_NAME *subject_name = X509_get_subject_name(peer_cert);
    if (!subject_name) {
        printf("Failed to get subject name from certificate\n");
        X509_free(peer_cert);
        return NULL;  // Failed to get subject name
    }
    return subject_name;
}

// Function to get the peer's IP address and check if it's localhost
int is_localhost(const char *peer_ip) {
    // Check if the IP address is localhost
    return (strcmp(peer_ip, "127.0.0.1") == 0 || strcmp(peer_ip, "::1") == 0);
}

// Function to get the peer's IP address from the subject name
const char *get_peer_ip(X509_NAME *subject_name) {
    // Retrieve the peer's IP address (replace this with your IP retrieval logic)
    X509_NAME_ENTRY *entry = X509_NAME_get_entry(subject_name, 0); // Assuming the first entry is the IP address
    if (!entry) {
        printf("Failed to get IP entry\n");
        return NULL;
    }

    ASN1_STRING *ip_asn1 = X509_NAME_ENTRY_get_data(entry);
    if (!ip_asn1) {
        printf("Failed to get IP ASN.1 string\n");
        return NULL;
    }

    // Retrieve the peer's IP address as a string
    return (const char *)ASN1_STRING_get0_data(ip_asn1);
}

// Function to get the peer's common name (CN) from the subject name
const char *get_peer_certificate_common_name(X509_NAME *subject_name) {
    int common_name_index = X509_NAME_get_index_by_NID(subject_name, NID_commonName, -1);
    if (common_name_index < 0) {
        printf("Common name not found\n");
        return NULL;  // Common name not found
    }

    printf("Common name index: %d\n", common_name_index);

    X509_NAME_ENTRY *common_name_entry = X509_NAME_get_entry(subject_name, common_name_index);
    if (!common_name_entry) {
        printf("Failed to get common name entry\n");
        return NULL;  // Failed to get common name entry
    }

    printf("Common name entry retrieved successfully\n");

    ASN1_STRING *common_name_asn1 = X509_NAME_ENTRY_get_data(common_name_entry);
    if (!common_name_asn1) {
        printf("Failed to get common name ASN.1 string\n");
        return NULL;  // Failed to get common name ASN.1 string
    }

    // Print the contents of the common name ASN.1 string
    printf("Common name ASN.1 string: %s\n", (const char *)ASN1_STRING_get0_data(common_name_asn1));

    // Convert the common name ASN.1 string to a C string
    return (const char *)ASN1_STRING_get0_data(common_name_asn1);
}
