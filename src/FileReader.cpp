#include "fileReader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "error.h"

std::string readFile(const std::string &fileName) {
	std::ifstream file(fileName);

	if (!file) exitWithError("Error: Could not open file " + fileName);

	std::stringstream buffer;

	buffer << file.rdbuf();

	file.close();

	return buffer.str();
}