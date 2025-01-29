#include "AST/NumericLiteral.h"

#include <iostream>

#include "Generator.h"

void NumericLiteral::print(const std::string &indent) const {
	std::cout << indent << getName() << " (" << getTypeName() << "): value = " << value
			  << std::endl;
}

void NumericLiteral::generateAssembly() {
	type = Type::NUMBER;
	std::string valueStr = std::to_string(value);

	std::string dataName = Generator::getDataName(valueStr);

	Generator::appendComment("Numeric Literal: " + valueStr);
	Generator::appendOutput("movq xmm0, QWORD [" + dataName + "]");
	Generator::push("xmm0");
}
