#ifndef DATABASE_H
#define DATABASE_H

#include <sys/types.h> // For types used in sys/stat.h and sys/unistd.h
#include <sys/stat.h>  // For stat()
#include <unistd.h>    // For mkdir()

#define MAX_PATH_LENGTH 256

// Global variable to store the data directory path
extern char datadir[MAX_PATH_LENGTH];

// Function to expand the DATABASE_DIR into a full path
void expand_path(const char *path);

// Function to create a directory if it does not exist
int make_dir(const char *dir);

// Function to recursively create directories
int initialize_database_directories(char *path);

#endif // DATABASE_H
