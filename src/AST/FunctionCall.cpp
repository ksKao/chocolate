
#include "AST/FunctionCall.h"

#include <iostream>

#include "AST/Identifier.h"

void FunctionCall::print(const std::string& indent) const {
	std::cout << indent << getName() << " (" << getTypeName() << "): " << std::endl;

	identifier->print(indent + "\t");

	std::cout << indent << "\tParameters: " << std::endl;

	for (const Expression& parameter : parameters) {
		parameter.print(indent + "\t");
	}
}

void FunctionCall::generateAssembly() {}
