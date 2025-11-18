
#include <cstdio>
#include <cstdlib>
#include "prog.h"

#include <filesystem>
#include <fstream>
#include <iostream>

int Program::process_asm_file(const fs::path& loc) {
    std::ifstream input_file(loc, std::ios::binary);
    if (!input_file) {
        std::cerr << "Failed to open file for reading: " << loc << std::endl;
        return EXIT_FAILURE;
    }
    std::string input_asm((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    input_file.close();

    std::string output_asm;
    Action action = parse_asm(output_asm, input_asm);
    if (this->verbose_mode()) {
        printf("Opened file \"%s\"\n", loc.string().c_str());
    }
    if (action == Action::Overwrite) {
        if (this->allowed_to_modify_files()) {
            std::ofstream output_file(loc, std::ios::binary | std::ios::trunc);
            if (!output_file) {
                std::cerr << "Failed to open file for writing: " << loc << std::endl;
                return EXIT_FAILURE;
            }
            output_file.write(output_asm.data(), output_asm.size());
        }
    }
    return 0;
}

int Program::process_file(const fs::path& loc) {
    string file_name = loc.string();
    if (file_name.ends_with(".c.src") || file_name.ends_with(".cpp.src")) {
        return process_asm_file(loc);
    }
    return 0;
}

int Program::process_directory(const fs::path& loc, int depth) {
    for (const auto& entry : fs::directory_iterator(loc)) {
        const fs::path& p = entry.path();
        if (entry.is_directory() && this->recursive_mode()) {
            int next_depth = depth + 1;
            if (!this->is_valid_depth(next_depth)) {
                continue;
            }
            int status = process_directory(p, next_depth);
            if (status != 0) {
                return status;
            }
            continue;
        }
        if (entry.is_regular_file()) {
            int status = process_file(p);
            if (status != 0) {
                return status;
            }
            continue;
        }
    }
    return 0;
}

int Program::process_path(char const * const path_str) noexcept {
    try {
        const fs::path loc(path_str);
        if (fs::is_directory(loc)) {
            return process_directory(loc);
        }
        if (fs::is_regular_file(loc)) {
            return process_file(loc);
        }
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Error: unknown exception" << std::endl;
        return EXIT_FAILURE;
    }
}
