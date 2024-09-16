#include "AST/NullLiteral.h"

#include <iostream>

#include "Generator.h"

void NullLiteral::print(const std::string &indent) const {
	std::cout << indent << getTypeName() << std::endl;
}

void NullLiteral::generateAssembly() const {
	Generator::appendOutput("movss xmm0, DWORD [rel null]");
}