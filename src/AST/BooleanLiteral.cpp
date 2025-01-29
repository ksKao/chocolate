#include "AST/BooleanLiteral.h"

#include <iostream>

#include "Generator.h"

void BooleanLiteral::print(const std::string& indent) const {
	std::cout << indent << getName() << " (" << getTypeName()
			  << "): value = " << (value ? "true" : "false") << std::endl;
}

void BooleanLiteral::generateAssembly() {
	type = Type::BOOLEAN;
	std::string valueStr = std::to_string(value);

	Generator::incrementStack();
	Generator::appendComment("Boolean Literal: " + valueStr);
	Generator::copyValueToStackFrom(std::to_string(value));
}