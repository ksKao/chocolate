#include "AST/NullLiteral.h"

#include <iostream>

#include "Generator.h"

void NullLiteral::print(const std::string &indent) const {
	std::cout << indent << getTypeName() << std::endl;
}

void NullLiteral::generateAssembly() const {
	Generator::appendOutput("movss xmm-1, DWORD [rel null]");
}