#include "Options.h"

#include "Error.h"

void Options::parse(int argc, char** argv) {
	if (argc < 2) {
		Error::abort("Please include the file path to compile.");
	}

	for (int i = 1; i < argc; i++) {
		std::string_view&& argument(argv[i]);
		std::string_view&& nextArgument(i < argc - 1 ? argv[i + 1] : "");

		if (i == 1) {
			inputFilename = argument;
		} else if (argument == "-o") {
			// check if has next argument
			if (nextArgument == "" || nextArgument.starts_with("-")) {
				Error::abort("Missing value for -o flag");
			} else {
				outputFilename = nextArgument;
				i++;
			}
		}
	}
}