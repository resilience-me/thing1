#include "command_defs.h"

int client_command_names_count() {
    int count = 0;
    while (client_command_names[count] != NULL) count++;  // Count commands
    return count;
}
