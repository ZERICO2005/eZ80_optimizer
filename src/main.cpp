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
    printf("\t--freestanding   : disable inlining of standard C functions\n");
    printf("\t--dry-run        : disable modifying files\n");
    printf("\t[-v | --verbose] : prints extra warnings/messages\n");
}

int main(int const argc, char const * const argv[]) {
    if (argc <= 1) {
        print_help();
        return EXIT_SUCCESS;
    }
    Program prog;

    // handle config
    int index = 1;
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
        if (strcmp(arg, "--verbose") == 0 || strcmp(arg, "-v") == 0) {
            prog.config.verbose = true;
            continue;
        }
        if (strcmp(arg, "--freestanding") == 0) {
            prog.config.freestanding = true;
            continue;
        }
        if (strcmp(arg, "--dry-run") == 0) {
            prog.config.dry_run = true;
            continue;
        }
        if (arg[0] == '-') {
            fprintf(stderr, "unknown option \"%s\"", arg);
            return EXIT_FAILURE;
        }
        break;
    }
    // handle files/folders/directories
    for (; index < argc; index++) {
        char const * const arg = argv[index];
        // printf("%d: \"%s\"\n", index, arg);
        int status = prog.process_path(arg);
        if (status != 0) {
            if (prog.config.verbose) {
                fprintf(stderr, "Failed with exit code: %d\n", status);
            }
            return EXIT_FAILURE;
        }
    }
    if (prog.total_diff.change_count == 0) {
        printf("no changes made\n");
    } else {
        printf(
            "made %d changes saving %d bytes\n",
            prog.total_diff.change_count,
            prog.total_diff.size_change
        );
    }
    return EXIT_SUCCESS;
}
