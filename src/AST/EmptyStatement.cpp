#include "AST/EmptyStatement.h"

#include <iostream>

void EmptyStatement::print(const std::string& indent) const {
	std::cout << indent << getName() << ": " << std::endl;
}

void EmptyStatement::generateAssembly() {}
