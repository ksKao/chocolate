#include "AST/NullLiteral.h"

#include <iostream>

#include "Generator.h"

void NullLiteral::print(const std::string &indent) const {
	std::cout << indent << getName() << " (" << getTypeName() << ")" << std::endl;
}

void NullLiteral::generateAssembly() {
	Generator::incrementStack();
	Generator::appendComment("Null literal");
	Generator::appendOutput("movsd xmm0, QWORD [null]");
	Generator::appendOutput("movsd QWORD [rsp], [null]");
}