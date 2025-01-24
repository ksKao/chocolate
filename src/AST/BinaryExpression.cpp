#include "AST/BinaryExpression.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void BinaryExpression::print(const std::string& indent) const {
	std::cout << indent << getTypeName() << ": " << op.getName() << std::endl;

	left->print(indent + "\t");
	right->print(indent + "\t");
}

void BinaryExpression::generateAssembly() const {
	Generator::appendComment("Generating left assembly");
	left->generateAssembly();

	Generator::appendComment("Generating right assembly");
	right->generateAssembly();

	// now, stack contains the values of left and right
	// pop them off and store them in xmm0 and xmm1 respectively
	Generator::appendComment("Getting pushed value from right and store in xmm1");
	Generator::appendOutput("movsd xmm1, QWORD [rsp]");
	Generator::decrementStack();
	Generator::appendComment("Getting pushed value from left and store in xmm0");
	Generator::appendOutput("movsd xmm0, QWORD [rsp]");
	Generator::decrementStack();

	// results are always stored in xmm0
	switch (op.type) {
		case TokenType::PLUS:
			Generator::appendComment("Binary Operator: +");
			Generator::appendOutput("addpd xmm0, xmm1");
			break;
		case TokenType::MINUS:
			Generator::appendComment("Binary Operator: -");
			Generator::appendOutput("subpd xmm0, xmm1");
			break;
		case TokenType::MULTIPLY:
			Generator::appendComment("Binary Operator: *");
			Generator::appendOutput("mulpd xmm0, xmm1");
			break;
		case TokenType::DIVIDE:
			Generator::appendComment("Binary Operator: /");
			Generator::appendOutput("divpd xmm0, xmm1");
			break;
		default:
			exitWithError("Operator " + op.value + " is not a valid binary operator.");
	}

	// store the result onto the stack
	Generator::incrementStack();
	Generator::appendOutput("movsd QWORD [rsp], xmm0");
}