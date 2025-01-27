#include "AST/UnaryExpression.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void UnaryExpression::print(const std::string& indent) const {
	std::cout << indent << getName() << " (" << getTypeName() << "): " << op.getName() << std::endl;

	operand->print(indent + "\t");
}

void UnaryExpression::generateAssembly() {
	operand->generateAssembly();

	if (operand->type != Type::NUMBER)
		Error::abort("Negative with " + operand->getTypeName() + " is not allowed");

	type = operand->type;

	Generator::appendComment("Negative unary operator");
	Generator::appendOutput(
		"movsd xmm1, QWORD [rsp]");	 // store the value from top of stack to xmm1
	Generator::decrementStack();
	Generator::appendOutput("xorpd xmm0, xmm0");  // zero out xmm0
	Generator::appendOutput("subpd xmm0, xmm1");  // subtract the original value from 0
	// store the subtracted value to top of stack
	Generator::incrementStack();
	Generator::appendOutput("movsd QWORD [rsp], xmm0");
}