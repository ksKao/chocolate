#include <sstream>

#include "FileHandler.h"
#include "Generator.h"
#include "Lexer.h"
#include "Options.h"
#include "Parser.h"

int main(int argc, char** argv) {
	Options::parse(argc, argv);

	std::string content = FileHandler::readFile(Options::inputFilename);

	Lexer lexer(content);
	std::vector<Token> tokens = lexer.tokenize();

	Parser parser(tokens);
	Scope program = parser.parse();

	FileHandler::writeFile(Generator::getOutput(program));

	program.print("");
}
