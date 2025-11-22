#include "../prog.h"

struct asm_line {
    vector<string>
};

void create_line(const vector<string>& token) {
    switch (token.size()) {
        default:
        case 0:
        {
            return;
        }
        case 1:
        {
            return;
        }
    }
    if (token.size() == 0) {
        return;
    }
    if (token.)
}

void parse_line(const string& input) {
    if (input.size() <= 1) {
        return;
    }
    std::istringstream iss(input);
    std::vector<std::string> out;
    std::string token;
    while (iss >> token) {
        out.push_back(token);
    }
    create_line(token);
}

void parse_all_lines(const vector<string>& input) {
    for (size_t i = 0; i < input.size(); i++) {
        parse_line(input[i]);
    }
}

void parse_asm(string& output, const string& input) {
    vector<string> lines;
    std::istringstream in(s);
    string line;
    while (getline(in, line)) {
        lines.push_back(line);
    }
    parse_all_lines(lines);

}
