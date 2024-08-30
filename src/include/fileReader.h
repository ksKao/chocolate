#pragma once
#include <string>

/// @brief Reads a file and returns the content in string
/// @param fileName A relative path to the file
/// @return Content of the file as string
std::string readFile(const std::string &fileName);