#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include "text.h"
#include "prog.h"

using namespace std;

struct asm_line {
    string str;
    size_t beg_pos;
    size_t end_pos;
};

static constexpr string::size_type Empty_Input_Length = 30;
static constexpr string::size_type Empty_Section_Length = 4;

static char const * const section_text_ax = "\tsection\t.text,\"ax\",@progbits\n";
static char const * const asm_code_start = ":\n";

static char const * const function_start[] {
	"\tpublic\t",
	"\tprivate\t",
	"\tweak\t",
};

static bool is_input_valid(const string& input) {
    if (input.length() < Empty_Input_Length) {
        printf("Input (length = %zu) is empty\n", input.length());
        return false;
    }
    return true;
}

static void process_code_block(string& block) {
    replace(block.begin(), block.end(), ' ', '#');
    replace(block.begin(), block.end(), '\t', '$');
}

static void extract_text_sections(string& output, const string &input) {
    const string marker = "\tsection\t.text,\"ax\",@progbits\n";
    output.clear();
    output.reserve(input.size());

    size_t pos = 0;
    int max_iter = 1024;
    for (; max_iter --> 0;) {
        // find next marker
        size_t start_marker = input.find(marker, pos);
        if (start_marker == string::npos) {
            // append remainder and finish
            output.append(input, pos);
            break;
        }

        // append everything up to and including the marker
        output.append(input, pos, start_marker - pos + marker.size());

        // compute content range
        size_t content_start = start_marker + marker.size();
        size_t next_marker = input.find(marker, content_start);
        if (next_marker == string::npos) {
            output.append(input, content_start);
            break;
        }
        size_t content_end = next_marker;

        // decide whether to process
        if (content_end > content_start && (content_end - content_start) >= Empty_Section_Length) {
            // extract block, let caller modify it, then append modified block
            string block = input.substr(content_start, content_end - content_start);
            process_code_block(block); // mutates block
            output += block;
        } else {
            // leave original content unchanged (including empty sections)
            output.append(input, content_start, content_end - content_start);
        }

        // continue searching after this section
        pos = content_end;
    }
    if (max_iter <= 0) {
        printf("Error: exhusted max_iter\n");
    }
}

void parse_asm(const string& input) {
    if (!is_input_valid(input)) {
        return;
    }
    string output;
    extract_text_sections(output, input);
    printf("%s", output.c_str());
}
