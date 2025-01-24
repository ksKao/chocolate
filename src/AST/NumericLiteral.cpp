#include "AST/NumericLiteral.h"

#include <iostream>

#include "Generator.h"

void NumericLiteral::print(const std::string &indent) const {
	std::cout << indent << getTypeName() << ": value = " << value << std::endl;
}

void NumericLiteral::generateAssembly() const {
	std::string valueStr = std::to_string(value);

	std::string dataName = Generator::getDataName(valueStr);

	Generator::incrementStack();
	Generator::appendComment("Numeric Literal: " + valueStr);
	Generator::appendOutput("movsd xmm0, QWORD [" + dataName + "]");
	Generator::appendOutput("movsd QWORD [rsp], xmm0");
}
