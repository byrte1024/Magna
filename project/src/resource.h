#pragma once

// resource.h

#include <stdio.h>
#include <string.h>
#include <errno.h>



#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#ifndef PATH_MAX
#define PATH_MAX 2600
#endif
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

static inline int ensure_path_exists(const char* path) {
    
    if (!path || *path == '\0') {
        errno = EINVAL;
        return -1;
    }

    char tmp[PATH_MAX];
    size_t len = strlen(path);
    if (len == 0 || len >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    snprintf(tmp, sizeof(tmp), "%s", path);  // portable

    // Remove trailing slash or backslash
    if (tmp[len - 1] == '/' || tmp[len - 1] == '\\') {
        tmp[len - 1] = '\0';
    }

    for (char* p = tmp + 1; *p; ++p) {
        if (*p == '/' || *p == '\\') {
            char save = *p;
            *p = '\0';
            if (MKDIR(tmp) != 0 && errno != EEXIST) {
                perror(tmp);  // Add this for debug
                return -1;
            }
            *p = save;
        }
    }

    if (MKDIR(tmp) != 0 && errno != EEXIST) {
        perror(tmp);
        return -1;
    }

    return 0;
}


//The data directory is where everything that needs to be loaded to/from disk is stored.
#define R_DIR_DATA "./data/"

//The data directory is split into smaller directories, (which contain their own)

//Static data will *not* be modified using the game, only loaded. This is where assets will be stored.
#define R_DIR_DATA_STATIC R_DIR_DATA "static/"

#define R_DIR_DATA_STATIC_ASSETS R_DIR_DATA_STATIC "assets/"

#define R_DIR_DATA_STATIC_BIN R_DIR_DATA_STATIC "bin/"

//Dynamic data will be modified using the game, this is where save files will be stored.
#define R_DIR_DATA_DYNAMIC R_DIR_DATA "dynamic/"

#define R_DIR_DATA_DYNAMIC_BIN R_DIR_DATA_DYNAMIC "bin/"

#define R_DIR_DATA_DYNAMIC_CONFIG R_DIR_DATA_DYNAMIC "config/"

//Temporary data is used for temporary files, like logs, caches, etc. this is data that might be cleared, dont store important stuff here, it wont last long.
#define R_DIR_DATA_TEMP R_DIR_DATA "temp/"

#define R_DIR_DATA_TEMP_LOGS R_DIR_DATA_TEMP "logs/"

#define R_DIR_DATA_TEMP_BIN R_DIR_DATA_TEMP "bin/"


static inline void ensureAllPathsExist() {
	if (ensure_path_exists(R_DIR_DATA) != 0) {
		fprintf(stderr, "Failed to ensure data directory exists\n");
	}
    if (ensure_path_exists(R_DIR_DATA_STATIC) != 0) {
        fprintf(stderr, "Failed to ensure static data directory exists\n");
    }
    if (ensure_path_exists(R_DIR_DATA_STATIC_ASSETS) != 0) {
        fprintf(stderr, "Failed to ensure static assets directory exists\n");
    }
    if (ensure_path_exists(R_DIR_DATA_STATIC_BIN) != 0) {
        fprintf(stderr, "Failed to ensure static bin directory exists\n");
    }
    if (ensure_path_exists(R_DIR_DATA_DYNAMIC) != 0) {
        fprintf(stderr, "Failed to ensure dynamic data directory exists\n");
    }
    if (ensure_path_exists(R_DIR_DATA_DYNAMIC_BIN) != 0) {
        fprintf(stderr, "Failed to ensure dynamic bin directory exists\n");
    }
    if (ensure_path_exists(R_DIR_DATA_DYNAMIC_CONFIG) != 0) {
        fprintf(stderr, "Failed to ensure dynamic config directory exists\n");
    }
    if (ensure_path_exists(R_DIR_DATA_TEMP) != 0) {
        fprintf(stderr, "Failed to ensure temp data directory exists\n");
    }
    if (ensure_path_exists(R_DIR_DATA_TEMP_LOGS) != 0) {
        fprintf(stderr, "Failed to ensure temp logs directory exists\n");
    }
    if (ensure_path_exists(R_DIR_DATA_TEMP_BIN) != 0) {
        fprintf(stderr, "Failed to ensure temp bin directory exists\n");
    }
    
}