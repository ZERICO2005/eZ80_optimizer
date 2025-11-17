#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "prog.h"

static void print_version(void) {
    printf("ezcxx_opt version 0.0.0\n");
}

static void print_help(void) {
    print_version();
    printf("Useage: ezcxx_opt [OPTIONS] <files/directories>\n");
    printf("Options:\n");
    printf("--freestanding: disable inlining of standard C functions\n");
}

int main(int const argc, char const * const argv[]) {
    if (argc <= 1) {
        print_help();
        return EXIT_SUCCESS;
    }
    int index = 1;
    Config config;
    // handle config
    for (; index < argc; index++) {
        char const * const arg = argv[index];
        if (strcmp(arg, "--version") == 0) {
            print_version();
            return EXIT_SUCCESS;
        }
        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            print_help();
            return EXIT_SUCCESS;
        }
        if (strcmp(arg, "--freestanding")) {
            config.freestanding = true;
            continue;
        }
        break;
    }
    // handle files/folders/directories
    for (; index < argc; index++) {
        char const * const arg = argv[index];
        int status = process_path(config, arg);
        if (status != 0) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
