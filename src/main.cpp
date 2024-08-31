#include "Error.h"
#include "FileReader.h"
#include "Lexer.h"
#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2)
        exitWithError("Program must have at least 1 argument");

    std::string content = readFile(argv[1]);

    Lexer lexer(content);
    std::vector<Token> token = lexer.tokenize();
    std::cout << content << std::endl;
}