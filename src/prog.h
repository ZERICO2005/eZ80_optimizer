#ifndef PROG_H
#define PROG_H

#include <string>
#include <cstdio>

using std::string;
using std::printf;

#if 1
#define test_printf printf
#else
#define test_printf(...)
#endif

enum class Action {
    Do_Nothing,
    Empty_Input = Do_Nothing,
    Invalid_Input = Do_Nothing,
    Overwrite,
};

struct Config {
    bool freestanding;
    Config() {
        freestanding = false;
    }
};

// The size of an empty *.c.src file
constexpr size_t Empty_Input_Length = 166 + 2;

#define ARRAY_LEN(x) (sizeof((x)) / sizeof((x)[0]))

Action parse_asm(string& output, const string& input);

int process_path(const Config& config, char const * const path);

#endif /* PROG_H */
