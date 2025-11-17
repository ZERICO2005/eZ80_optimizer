#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include "text.h"
#include "prog.h"
#include "asm_pattern.h"
#include "text_util.h"

using namespace std;

static constexpr size_t Empty_Code_Length = 4;

static char const * const section_text_ax = "\n\tsection\t.text,\"ax\",@progbits\n";

struct find_and_replace {
    string& str;
    find_and_replace(std::string &other) : str(other) {}
    int replace(char const * const src, char const * const dst) {
        const size_t src_len = strlen(src);
        const size_t dst_len = strlen(dst);
        int count = 0;
        size_t pos = 0;
        while((pos = str.find(src, pos)) != std::string::npos) {
            str.replace(pos, src_len, dst);
            pos += dst_len; // Handles case where 'to' is a substring of 'from'
            count++;
        }
        return count;
    }
};

static int process_code_block(string& block) {
    find_and_replace code(block);
    int total_changes = 0;
    for (size_t i = 0; i < ARRAY_LEN(asm_pattern); i++) {
        const Pattern& pat = asm_pattern[i];
        int count = code.replace(pat.src, pat.dst);
        if (count > 0) {
            printf(
                "Replaced %d:\n\t%s\n\t%s\n",
                count,
                visualize_escape_codes(pat.src).c_str(),
                visualize_escape_codes(pat.dst).c_str()
            );
        }
        total_changes += count;
    }
    return total_changes;
}

Action parse_asm(string& output, const string& input) {
    if (input.size() < Empty_Input_Length) {
        return Action::Empty_Input;
    }
    size_t beg_pos, end_pos;
    beg_pos = input.find(section_text_ax);
    if (beg_pos == string::npos) {
        return Action::Invalid_Input;
    }
    end_pos = input.rfind(section_text_ax);
    if (end_pos == string::npos) {
        return Action::Invalid_Input;
    }
    if (beg_pos == end_pos) {
        return Action::Do_Nothing;
    }
    if (end_pos < beg_pos) {
        return Action::Do_Nothing;
    }
    if (end_pos - beg_pos < Empty_Code_Length) {
        // too small to process
        return Action::Do_Nothing;
    }

    string code_block = input.substr(beg_pos, end_pos - beg_pos);

    int changes = process_code_block(code_block);

    printf("Total changes: %d\n", changes);

    if (changes == 0) {
        return Action::Do_Nothing;
    }

    output = input.substr(0, beg_pos);
    output += code_block;
    output += input.substr(end_pos);
    return Action::Overwrite;
}
