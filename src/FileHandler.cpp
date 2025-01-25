#include "FileHandler.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Error.h"
#include "Options.h"

std::string FileHandler::readFile(const std::string &fileName) {
	std::ifstream file(fileName);

	if (!file) Error::abort("Error: Could not open file " + fileName);

	std::stringstream buffer;

	buffer << file.rdbuf();

	file.close();

	return buffer.str();
}

void FileHandler::writeFile(const std::stringstream &content) {
	std::ofstream outFile(Options::outputFilename);

	if (outFile.is_open()) {
		outFile << content.str();
		outFile.close();
	} else {
		Error::abort("Failed to open output file: " + Options::outputFilename);
	}
}
