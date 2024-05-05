// #include "command_defs.h"
// // #include "server_as_server_command_handlers.h"
// #include <stddef.h>

// #include "command_defs.h"
// #include "server_as_client_command_handlers.h"
// #include <stddef.h>

// #include "client_commands.h"  // Include the header file for declarations
// #include "client_command_handlers.h"
// #include "command_defs.h"

// // Arrays of function pointers to command handlers
// void *server_as_client_command_handlers[] = {
//     (void*)handle_send_account_exists_query,
//     NULL  // Include NULL for completeness
// };
// void *server_as_server_command_handlers[] = {
//     NULL,  // Handler not implemented yet
//     NULL  // Include NULL for completeness
// };
// void *client_command_handlers[] = {
//     (void*)handle_login_user,
//     (void*)handle_register_user,
//     (void*)handle_logout_user,
//     (void*)handle_delete_user,
//     (void*)handle_add_connection,
//     NULL  // Include NULL for completeness
// };

// // Constructing the commands arrays
// Command *server_as_client_commands = NULL;
// Command *server_as_server_commands = NULL;
// Command *client_commands = NULL;
