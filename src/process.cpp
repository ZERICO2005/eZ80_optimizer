
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <dirent.h>
#include <sys/stat.h>
#include "prog.h"

#include <sstream>
#include <fstream>

static void load_asm_file(string& output, char const * const path) {
    std::ifstream file(path);
    if (!file) {
        fprintf(stderr, "Error opening file: \"%s\"\n", path);
    }
    output.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return;
}

static void save_asm_file(std::string& input, char const * const path) {
    std::ofstream file(path, std::ios::binary);

    if (!file) {
        fprintf(stderr, "Error opening file for writing: \"%s\"\n", path);
        return;
    }
    file.write(input.c_str(), input.size());
    file.close();
}

int process_file(const Config& config, char const * const path) {
	string input_asm;
	string output_asm;
	load_asm_file(input_asm, path);
	Action action = parse_asm(output_asm, input_asm);
	if (action == Action::Overwrite && !config.dry_run) {
        printf("WRITE: %s\n", path);
		save_asm_file(output_asm, path);
	}
    return 0;
}

static bool path_ends_with(const char *s, const char *suffix) {
    size_t n = strlen(s), m = strlen(suffix);
    return (n >= m && strcmp(s + n - m, suffix) == 0);
}

int process_directory(const Config& config, char const * const path) {
    DIR *dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "opendir(%s) failed: %s\n", path, strerror(errno));
        return EXIT_FAILURE;
    }
    struct dirent *entry;
    int files_found = 0;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (!(path_ends_with(entry->d_name, ".c.src") || path_ends_with(entry->d_name, ".cpp.src"))) {
            continue;
        }
        char file_path[PATH_MAX];
        struct stat st;

        if (snprintf(file_path, sizeof(file_path), "%s/%s", path, entry->d_name) >= PATH_MAX) {
            fprintf(stderr, "path exceeds PATH_MAX(%d): \"%s/%s\"\n", PATH_MAX, path, entry->d_name);
            closedir(dir);
            return EXIT_FAILURE;
        }
        files_found++;
        int status = process_file(config, file_path);
        if (status != 0) {
            closedir(dir);
            return status;
        }
    }

    closedir(dir);
    return 0;
}

enum class path_type {
    unknown,
    directory,
    file,
};

static path_type get_path_type(char const * const path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return path_type::unknown;
    }
    if (S_ISDIR(st.st_mode)) {
        return path_type::directory;
    }
    if (S_ISREG(st.st_mode)) {
        return path_type::file;
    }
    return path_type::unknown;
}

int process_path(const Config& config, char const * const path) {
    switch (get_path_type(path)) {
        default:
        case path_type::unknown: {
            fprintf(stderr, "Unknown path type: \"%s\"\n", path);
            return EXIT_FAILURE;
        } break;
        case path_type::directory: {
            return process_directory(config, path);
        } break;
        case path_type::file: {
            return process_directory(config, path);
        } break;
    }
}
