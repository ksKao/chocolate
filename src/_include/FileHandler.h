#pragma once
#include <string>

namespace FileHandler {
/// @brief Reads a file and returns the content in string
/// @param fileName A relative path to the file
/// @return Content of the file as string
std::string readFile(const std::string& fileName);

void writeFile(const std::stringstream& content);
}  // namespace FileHandler