#include "fileReader.h"
#include "error.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

std::string readFile(const std::string &fileName)
{
    std::ifstream file(fileName);

    if (!file)
        exitWithError("Error: Could not open file " + fileName);

    std::stringstream buffer;

    buffer << file.rdbuf();

    file.close();

    return buffer.str();
}