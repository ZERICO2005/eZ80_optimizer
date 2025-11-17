#ifndef PROG_H
#define PROG_H

#include <string>

using std::string;

enum class Action {
    Do_Nothing,
    Empty_Input = Do_Nothing,
    Invalid_Input = Do_Nothing,
    Overwrite,
};

// The size of an empty *.c.src file
constexpr size_t Empty_Input_Length = 166 + 2;

#define ARRAY_LEN(x) (sizeof((x)) / sizeof((x)[0]))

Action parse_asm(string& output, const string& input);

#endif /* PROG_H */
