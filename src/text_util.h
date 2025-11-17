#ifndef TEXT_UTIL_H
#define TEXT_UTIL_H

#include <string>

using std::string;

#include <stdio.h>
#include <ctype.h>

inline string visualize_escape_codes(const string& str) {
	string ret = "";
    for (char ch : str) {
		if (ch == '\'') {
			ret += "\\\'";
			continue;
		}
		if (ch == '\"') {
			ret += "\\\"";
			continue;
		}
		if (ch == '\\') {
			ret += "\\\\";
			continue;
		}
        if (isprint(ch)) {
            ret += ch;
			continue;
        }
		switch (ch) {
			case '\0': { ret += "\\0"; } break;
			case '\t': { ret += "\\t"; } break;
			case '\n': { ret += "\\n"; } break;
			case '\r': { ret += "\\r"; } break;
			case '\f': { ret += "\\f"; } break;
			case '\v': { ret += "\\v"; } break;
			default: {
				char buf[10];
				snprintf(buf, sizeof(buf), "\\x%02X", ch);
				ret += buf;
			} break;
		}
    }
	return ret;
}

#endif /* TEXT_UTIL_H */
