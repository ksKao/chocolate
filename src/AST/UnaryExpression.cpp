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

	Generator::appendComment("Unary operator: " + op.getName());

	switch (operand->type) {
		case Type::NUMBER: {
			if (op.type == TokenType::MINUS) {
				Generator::appendOutput(
					"movsd xmm1, QWORD [rsp]");	 // store the value from top of stack to xmm1
				Generator::decrementStack();
				Generator::appendOutput("xorpd xmm0, xmm0");  // zero out xmm0
				Generator::appendOutput("subpd xmm0, xmm1");  // subtract the original value from 0
				// store the subtracted value to top of stack
				Generator::incrementStack();
				Generator::appendOutput("movsd QWORD [rsp], xmm0");
			} else {
				Error::abort("Unary expression " + op.getName() + " with type " +
							 operand->getTypeName() + " is not allowed.");
			}
			break;
		}
		case Type::BOOLEAN: {
			if (op.type == TokenType::NOT) {
				Generator::appendOutput("mov rax, QWORD [rsp]");
				Generator::appendOutput("not rax");
				Generator::appendOutput("mov QWORD [rsp], rax");
			} else {
				Error::abort("Unary expression " + op.getName() + " with type " +
							 operand->getTypeName() + " is not allowed.");
			}
			break;
		}
		default: {
			Error::abort("Unary expression " + op.getName() + " with type " +
						 operand->getTypeName() + " is not allowed.");
			break;
		}
	}
}