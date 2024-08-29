#include "error.h"
#include "fileReader.h"
#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2)
        exitWithError("Program must have at least 1 argument");

    std::string content = readFile(argv[1]);

    std::cout << content << std::endl;
}