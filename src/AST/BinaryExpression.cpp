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
	left->generateAssembly();

	// move the value stored in xmm0 to xmm1 to make space for value stored in right
	Generator::appendOutput("movapd xmm1, xmm0");

	right->generateAssembly();

	// now, left's value is stored at xmm1 and right's value is stored at xmm0
	// so we check what is the operator and generate the corresponding instruction and store in xmm0;
	switch (op.type) {
		case TokenType::PLUS:
			Generator::appendOutput("addpd xmm0, xmm1");
			break;
		case TokenType::MINUS:
			// xmm1 = xmm1 - xmm0 (xmm1 = left, xmm0 = right)
			// then move the result into xmm0 for use by other nodes
			Generator::appendOutput("subpd xmm1, xmm0");
			Generator::appendOutput("movapd xmm0, xmm1");
			break;
		case TokenType::MULTIPLY:
			Generator::appendOutput("mulpd xmm0, xmm1");
			break;
		case TokenType::DIVIDE:
			Generator::appendOutput("divpd xmm1, xmm0");
			Generator::appendOutput("movapd xmm0, xmm1");
			break;
		default:
			exitWithError("Operator " + op.value + " is not a valid binary operator.");
	}
}