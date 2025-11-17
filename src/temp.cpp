#include <stdio.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>

#include "prog.h"

using std::string;

void load_asm_file(string& output, const char* input_path) {
    std::ifstream file(input_path);
    if (file) {
        // Read the file content into the string
        output.assign((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        file.close(); // Close the file
    } else {
        std::cerr << "Error opening file: " << input_path << std::endl;
    }
}

void save_asm_file(const char* output_path, std::string& input) {
    std::ofstream output_file(output_path, std::ios::binary);

    if (!output_file) {
        std::cerr << "Error opening file for writing: " << output_path << std::endl;
        return;
    }

    output_file.write(input.c_str(), input.size());

    output_file.close();

    std::cout << "Data written to " << output_path << " successfully." << std::endl;
}

int main(void) {
	char const * const input_path = "../input/input_asm.src";
	char const * const output_path = "../output/output_asm.src";
	printf("asm parse\n");

	string input_asm;
	string output_asm;
	load_asm_file(input_asm, input_path);
	if (input_asm.empty() || input_asm.size() < 10) {
		printf("Error: \"%s\" is empty\n", input_path);
		return 0;
	}
	Action action = parse_asm(output_asm, input_asm);
	if (action == Action::Overwrite) {
		save_asm_file(output_path, output_asm);
	}

	printf("finished\n");
	return 0;
}
