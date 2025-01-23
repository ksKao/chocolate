#include "AST/PrintStatement.h"

#include <iostream>

#include "Generator.h"

void PrintStatement::print(const std::string& indent) const {
	std::cout << indent << getTypeName() << std::endl;

	value->print(indent + "\t");
}

void PrintStatement::generateAssembly() const {
	value->generateAssembly();

	Generator::appendOutput("; Print Statement: print from top of stack");
	Generator::appendOutput("movsd xmm0, QWORD [rsp]");
	Generator::appendOutput("mov rdi, float_format");
	Generator::appendOutput("mov eax, 1");
	Generator::appendOutput("call printf");
	Generator::decrementStack();
}