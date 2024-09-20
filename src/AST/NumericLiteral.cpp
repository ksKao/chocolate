#include "AST/NumericLiteral.h"

#include <iostream>

#include "Generator.h"

void NumericLiteral::print(const std::string &indent) const {
	std::cout << indent << getTypeName() << ": value = " << value << std::endl;
}

void NumericLiteral::generateAssembly() const {
	std::string dataName = Generator::getDataName(std::to_string(value));
	Generator::appendOutput("movsd xmm0, QWORD [" + dataName + "]");
}
