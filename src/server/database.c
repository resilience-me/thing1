#include "database.h"
#include "server_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <errno.h>

// Allocate memory for the global variable
char datadir[MAX_PATH_LENGTH];

// Function to replace the first occurrence of ~ with the home directory
void expand_path(const char *path) {
    if (path[0] == '~') {
        const char *home = getenv("HOME");
        if (home) {
            if (strlen(home) + strlen(path) - 1 < sizeof(datadir)) {
                strcpy(datadir, home);
                strcat(datadir, path + 1); // skip the tilde
            } else {
                printf("Path length exceeds buffer size\n");
            }
        } else {
            printf("HOME environment variable not set\n");
        }
    } else {
        if (strlen(path) < sizeof(datadir)) {
            strcpy(datadir, path);
        } else {
            printf("Path length exceeds buffer size\n");
        }
    }
}

// Function to create a directory if it does not exist
int make_dir(const char *dir) {
    struct stat st = {0};
    if (stat(dir, &st) == 0) { // Check if directory exists
        return 1; // Directory already exists
    }

    if (mkdir(dir, 0700) == -1) { // Permissions set to read/write/execute for owner
        perror("Failed to create directory");
        return -1; // Directory creation failed
    }
    
    return 0; // Directory created successfully
}

// Function to recursively create directories
int make_dirs(const char *orig_path) {
    char *path = strdup(orig_path);
    char *p = path + strlen(path);

    // Move backwards and create each directory
    while (p > path && *p != '/') p--;  // find the last slash
    *p = '\0';  // temporarily end string here

    int result = make_dirs(path);
    if (p != path && result != 0) {  // Recur first
        free(path);
        return result;
    }
    int result = make_dir(orig_path);
    free(path);
    return result;
}

int initialize_database_directories() {
    expand_path(DATABASE_DIR);
    if (strlen(datadir) == 0) {
        printf("Failed to expand path\n");
        return -1;
    }

    // Try to create directory
    if (make_dirs(datadir) == -1) {
        return -1;
    }
    printf("Directories created successfully at %s\n", datadir);
    return 0;
}
