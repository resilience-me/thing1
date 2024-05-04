# Ryan Fugger's Ripple in P2P way

A project to implement Ryan's idea in a true P2P way. The system is similar to email, in that anyone can run a server, and servers can also host multiple accounts if they want to.

### Client and server

The server interacts with other servers, with TCP/IP and asymmetric encryption. The server also interacts with the client. The client interacts with users over TCP/IP + asymmetric handshake, as `client server.xyz`. This client can interface a website, via web socket. The port is 2012 by default.

The repository structure is:

    src/
    │
    ├── common/ # Common code shared between client and server
    │
    ├── client/ # Source files for the client application
    │
    └── server/ # Source files for the server application
        │
        ├── client_services/ # Source files for client services on the server
        │
        └── server_services/ # Source files for server-specific services
            │
            ├── common/ # Common code shared between server_as_client and server_as_server
            │
            ├── server_as_client/ # Source files for server acting as a client
            │
            └── server_as_server/ # Source files for server acting as a server

### Encryption

The platform allows anyone to host a server, and a server can have multiple accounts. Servers interact over asymmetric handshakes. Clients interact with servers over asymmetric handshakes. Message authentication code are also used. In theory Ripple can rely on symmetric encryption only and things like one-time pads, giving the strongest possible encryption, but for practical purposes asymmetric keys are used in this implementation (although adding support for symmetric keys is recommended eventually. )

### Database

The "database" is simply a directory "accounts", that stores directories for each account (named with account name), and text files with account state. A special default account can be referenced by an empty name (good for single-account servers, as transactions then just become the server identifier part, @server.xyz, without the account identifier. ) It is set up with #define DEFAULT_ACCOUNT "default" (using whatever name you want... )

Accounts on the server are password protected, so any interaction with an account (such as sending a payment request) requires password authentication. The passwords are simply stored in text files in the account folders. Authentication is done via the client and the `LOGIN` command.

### Routing

The routing is very simple. It is practically “stateless”, no routing tables are stored, all routing is generated for each payment request. The benefit is that paths change constantly in Ripple (as trust lines fill up or credit is cleared), so a “routing table” would not reflect the true state anyway. While “state” in routing may be beneficial (and normally used in internet routing), a “stateless” routing mechanism is simple to build, and a good start. It can be expanded upon later.

The path-finding optimizes for never going too deep. It is bidirectional, reducing accounts queried to 2*sqrt(unidirectional). And, it searches in increments of 1, always returning to the root before increasing the depth by 1 (the root then sends out a new request, with depth incremented by 1. ) Thus, whenever a path is found, the search ends (the root stops replying to response by incrementing request. ) While it may seem wasteful to constantly go back and forth, it saves up for the added cost by never going deeper than it has to. Path requests use an identifier that is a simple random number, and are sent both from buyer and receiver. Whenever these “fronts” meet, a path is found, and the first path found is chosen. To enforce the “return to root before incrementing” approach, accounts should only accept queries that grow in increments of 1. (Though in the future, other path finding mechanisms may use different approaches, but this is a good first one. )

Thus the path-finding is not actually aware of the sender or receiver account address. This is not for privacy reasons, it is just because it is redundant to provide accounts with that information in this routing system.

### Data structures for routing

The routing is centered around caches that keep track of paths an account is involved in searching for. Accounts track when they’re currently involved in a request, and they track the depth they are at for the request. Technically, linked lists are used, and linear search. During linear search (to either find a path identifier within an account’s caches, or an account within the overall routing cache) old queries are also cleared, and accounts with no active queries are cleared.

    #define CACHE_RETENTION_SECONDS 300

    typedef struct CacheEntry {
        time_t timestamp;
        int depth;
        struct CacheEntry *next;
    } CacheEntry;

    typedef struct PathCacheEntry {
        int identifier;
        CacheEntry *incomingHead;
        CacheEntry *outgoingHead;
        char *nextHop;
        struct PathCacheEntry *next;
    } PathCacheEntry;

    typedef struct AccountNode {
        char *accountId;
        PathCacheEntry *cacheHead;
        struct AccountNode *next;
    } AccountNode;

    AccountNode *head = NULL;

### Payment

Once a path is found, payment is made from sender to receiver. It propagates along the path, and each account commits to the payment (locking their credit line to a value), and then a "success" signal propagates back from the receiver, and each account records the payment as having been made. The payment is logged to a log file "payments.txt", including a timestamp. People can easily view their payment history with this log file.
