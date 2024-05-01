#include "database.h"
#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

// Function to create directories if they don't exist
void create_directories() {
    char *dirs[] = { DATABASE_DIR, DATABASE_DIR "/accounts" };
    for (size_t i = 0; i < sizeof(dirs) / sizeof(dirs[0]); ++i) {
        // Expand tilde in path
        char path[256];
        snprintf(path, sizeof(path), "%s", dirs[i]);
        if (path[0] == '~') {
            const char *home = getenv("HOME");
            if (home != NULL) {
                snprintf(path, sizeof(path), "%s%s", home, dirs[i] + 1);
            }
        }

        // Check if directory already exists
        struct stat st;
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
            // Directory already exists, skip creation
            continue;
        }

        // Create directory
        if (mkdir(path, 0777) == -1 && errno != EEXIST) {
            fprintf(stderr, "Failed to create directory %s: %s\n", path, strerror(errno));
            // Handle error
        }
    }
}
