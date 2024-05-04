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
    if (stat(dir, &st) == -1) { // Check if directory exists
        if (mkdir(dir, 0700) == -1) { // Permissions set to read/write/execute for owner
            perror("Failed to create directory");
            return -1;
        }
    }
    return 0;
}

// Function to recursively create directories
int make_dirs(const char *orig_path) {
    char *path = strdup(orig_path);
    char *p = path + strlen(path);

    // Move backwards and create each directory
    while (p > path && *p != '/') p--;  // find the last slash
    *p = '\0';  // temporarily end string here

    if (p != path && make_dirs(path) != 0) {  // Recur first
        free(path);
        return -1;
    }

    if (mkdir(orig_path, 0700) == -1 && errno != EEXIST) {
        perror("Failed to create directory");
        free(path);
        return -1;
    }

    free(path);
    return 0;
}

int initialize_database_directories() {
    expand_path(DATABASE_DIR);
    if (strlen(datadir) == 0) {
        printf("Failed to expand path\n");
        return 1;
    }

    // Check if the database directory already exists
    struct stat st;
    if (stat(datadir, &st) == 0) {
        return 0;  // Directory exists, no need to create
    }

    // Directory does not exist, try to create it
    if (make_dirs(datadir) == -1) {
        return 1;
    }
    printf("Directories created successfully at %s\n", datadir);
    return 0;
}
