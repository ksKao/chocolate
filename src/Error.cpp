#include "Error.h"

#include <iostream>

void Error::abort(const std::string &message) {
	std::cerr << message << std::endl;
	exit(EXIT_FAILURE);
}