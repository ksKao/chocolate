#include "error.h"
#include <iostream>

void exitWithError(const std::string &message)
{
    std::cerr << message << std::endl;
    exit(EXIT_FAILURE);
}