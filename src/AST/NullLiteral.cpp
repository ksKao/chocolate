#include "AST/NullLiteral.h"

#include <iostream>

#include "Generator.h"

void NullLiteral::print(const std::string &indent) const {
	std::cout << indent << getTypeName() << std::endl;
}

void NullLiteral::generateAssembly() const {
	Generator::incrementStack();
	Generator::appendOutput("; Null literal");
	Generator::appendOutput("movsd xmm0, QWORD [null]");
	Generator::appendOutput("movsd QWORD [rsp], [null]");
}