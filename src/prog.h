#ifndef PROG_H
#define PROG_H

#include <string>
#include <cstdio>
#include <filesystem>
#include <algorithm>

using std::string;
using std::printf;
namespace fs = std::filesystem;

enum class Action {
    Do_Nothing,
    Empty_Input = Do_Nothing,
    Invalid_Input = Do_Nothing,
    Overwrite,
};

struct Optimize_Diff {
    int change_count;
    int size_change;
    Optimize_Diff() {
        change_count = 0;
        size_change = 0;
    }
    Optimize_Diff& operator+=(const Optimize_Diff& other) {
        this->change_count += other.change_count;
        this->size_change += other.size_change;
        return *this;
    }
};

struct Config {
    bool freestanding;
    bool dry_run;
    bool verbose;
    bool recursive;
    int max_depth;
    static constexpr int default_max_depth = 4;
    static constexpr int highest_max_depth = 32;
    void set_max_depth(int depth) {
        max_depth = std::clamp(0, depth, highest_max_depth);
    }
    bool is_valid_depth(int depth) const {
        if (depth < 0 || depth > max_depth) {
            return false;
        }
        return true;
    }
    Config() {
        freestanding = false;
        dry_run = false;
        verbose = false;
        recursive = false;
        max_depth = default_max_depth;
    }
};

struct Program {
    Config config;
    Optimize_Diff total_diff;

    Action parse_asm(string& output, const string& input);
    int process_path(char const * path) noexcept;

    int process_directory(const fs::path& loc, int depth = 0);
    int process_file(const fs::path& loc);
    int process_asm_file(const fs::path& loc);
    bool recursive_mode() const {
        return config.recursive;
    }
    bool verbose_mode() const {
        return config.verbose;
    }
    bool allowed_to_modify_files() const {
        return !config.dry_run;
    }
    int get_max_depth() const {
        return config.max_depth;
    }
    bool is_valid_depth(int depth) const {
        return config.is_valid_depth(depth);
    }
};

// The size of an empty *.c.src file
constexpr size_t Empty_Input_Length = 166 + 2;

#define ARRAY_LEN(x) (sizeof((x)) / sizeof((x)[0]))

Action parse_asm(string& output, const string& input);

#endif /* PROG_H */
