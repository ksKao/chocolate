#include "AST/NullLiteral.h"

#include <iostream>

#include "Generator.h"

void NullLiteral::print(const std::string &indent) const {
	std::cout << indent << getName() << " (" << getTypeName() << ")" << std::endl;
}

void NullLiteral::generateAssembly() {
	if (isStatement) return;

	Generator::appendComment("Null literal");
	Generator::appendOutput("movq xmm0, [null]");
	Generator::push("xmm0");
}