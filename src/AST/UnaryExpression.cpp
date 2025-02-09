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

	type = operand->type;

	if (isStatement) return;

	Generator::appendComment("Unary operator: " + op.getName());

	std::string errorMessage =
		"Unary expression " + op.getName() + " with type " + operand->getTypeName() + " is not allowed.";

	switch (operand->type) {
		case Type::NUMBER: {
			if (op.type == TokenType::MINUS) {
				Generator::pop("xmm1");
				Generator::appendOutput("xorpd xmm0, xmm0");  // zero out xmm0
				Generator::appendOutput("subpd xmm0, xmm1");  // subtract the original value from 0
				Generator::push("xmm0");
			} else {
				Error::abortWithLineNumber(errorMessage, op.lineNumber);
			}
			break;
		}
		case Type::BOOLEAN: {
			if (op.type == TokenType::NOT) {
				Generator::copyValueFromStackTo("rax");
				Generator::appendOutput("not rax");
				Generator::copyValueToStackFrom("rax");
			} else {
				Error::abortWithLineNumber(errorMessage, op.lineNumber);
			}
			break;
		}
		default: {
			Error::abortWithLineNumber(errorMessage, op.lineNumber);
			break;
		}
	}
}