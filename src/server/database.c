#include "database.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

char datadir[256]; // Global variable to store the data directory path

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
int make_dirs(char *path) {
    char *subpath = path;
    char *pos = strchr(subpath, '/');
    while (pos != NULL) {
        *pos = '\0';
        if (make_dir(path) == -1) return -1;
        *pos = '/';
        subpath = pos + 1;
        pos = strchr(subpath, '/');
    }
    if (*subpath) {
        if (make_dir(path) == -1) return -1; // Create last segment
    }
    return 0;
}

// Function to ensure database directories exist; creates them if they don't
int initialize_database_directories() {
    expand_path(DATABASE_DIR);
    if (strlen(datadir) == 0) {
        printf("Failed to expand path\n");
        return 1;
    }
    if (make_dirs(datadir) == -1) {
        return 1;
    }
    printf("Directories created successfully at %s\n", datadir);
    return 0;
}
