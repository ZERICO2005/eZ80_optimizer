
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <dirent.h>
#include <sys/stat.h>
#include "prog.h"


static int is_directory(char const * const path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    return S_ISDIR(st.st_mode);
}

int process_file() {

}

int process_directory() {

}

int type_of(const char *path) {
    struct stat st;
    if (lstat(path, &st) != 0) return -1;
    if (S_ISDIR(st.st_mode)) return 1;   // directory
    if (S_ISREG(st.st_mode)) return 2;   // regular file
    if (S_ISLNK(st.st_mode)) return 3;   // symlink
    return 0; // other (socket, fifo, block, char)
}

int process_path(const Config& config, char const * const path) {
    if ()
    DIR *dir = opendir(dirpath);
}
