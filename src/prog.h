#ifndef PROG_H
#define PROG_H

#include <string>

using std::string;

#define ARRAY_LEN(x) (sizeof((x)) / sizeof((x)[0]))

void parse_asm(const string& input);

#endif /* PROG_H */
