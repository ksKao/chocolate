#include "Error.h"
#include "FileReader.h"
#include "Lexer.h"
#include "Parser.h"
#include "Generator.h"

#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2)
        exitWithError("Program must have at least 1 argument");

    std::string content = readFile(argv[1]);

    Lexer lexer(content);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    Program program = parser.parse();
    std::cout << Generator::getOutput(program) << std::endl;

    program.print("");
}