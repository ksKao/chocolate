#include "error.h"
#include <iostream>

void exitWithError(std::string message)
{
    std::cerr << message << std::endl;
    exit(EXIT_FAILURE);
}