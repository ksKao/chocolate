#include "Error.h"

#include <iostream>

void Error::abort(const std::string &message) {
	std::cerr << message << std::endl;
	exit(EXIT_FAILURE);
}

void Error::abortWithLineNumber(const std::string &message, size_t lineNo) {
	std::cerr << "[ERR] Line " << lineNo << ":\t" << message << std::endl;
	exit(EXIT_FAILURE);
}