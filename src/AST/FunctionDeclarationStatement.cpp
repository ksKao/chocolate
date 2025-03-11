#include "AST/FunctionDeclarationStatement.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void FunctionDeclarationStatement::print(const std::string& indent) const {
	std::cout << indent << getName() << ": name = " << identifier->token.value << std::endl;

	scope->print(indent + "\t");
}

void FunctionDeclarationStatement::generateAssembly() {
	std::shared_ptr<Function> function = Generator::getFunction(identifier->token.value);

	if (function == nullptr)
		Error::abortWithLineNumber("Could not get " + identifier->token.value + " after declaration",
								   functionToken.lineNumber);

	Generator::functionTrace.push_back(function);

	for (const std::unique_ptr<Node>& statement : scope->statements) {
		statement->generateAssembly();
	}

	Generator::appendOutput("ret");

	Generator::functionTrace.pop_back();
}
