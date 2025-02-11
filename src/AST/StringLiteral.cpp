#include "AST/StringLiteral.h"

#include <iostream>

#include "Generator.h"

void StringLiteral::print(const std::string& indent) const {
	std::cout << indent << getName() << " (" << getTypeName() << "): value = " << value << std::endl;
}

void StringLiteral::generateAssembly() {
	type = Type::STRING;

	if (isStatement) return;

	std::string dataName = Generator::getDataName("'" + value + "', 0", "db");

	Generator::appendComment("String Literal: " + value);
	Generator::push(dataName);
}