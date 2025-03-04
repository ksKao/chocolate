#include "AST/FunctionDeclarationStatement.h"

#include <iostream>

void FunctionDeclarationStatement::print(const std::string &indent) const {
	std::cout << indent << getName() << ": name = " << identifier->token.value << std::endl;

	scope->print(indent + "\t");
}

void FunctionDeclarationStatement::generateAssembly() {}
