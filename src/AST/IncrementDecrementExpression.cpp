#include "AST/IncrementDecrementExpression.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void IncrementDecrementExpression::print(const std::string& indent) const {
	std::cout << indent << getName() << " (" << getTypeName() << "): " << op.getName() << std::endl;

	identifier->print(indent + "\t");
}

void IncrementDecrementExpression::generateAssembly() {
	Generator::appendComment("Increment/Decrement");
	identifier->generateAssembly();
	Generator::pop("xmm0");

	Variable* variable = Generator::getVariable(identifier->token.value);

	// check if identifier has been declared
	if (variable == nullptr)
		Error::abortWithLineNumber("Identifier `" + identifier->token.value + "` has not been declared.",
								   identifier->token.lineNumber);

	if (identifier->type != Type::NUMBER)
		Error::abortWithLineNumber("Could not call " + op.getName() + " with type " + identifier->getTypeName(),
								   op.lineNumber);

	if (!isPrefix && !isStatement) {
		Generator::push("xmm0");
	}

	type = identifier->type;

	switch (op.type) {
		case TokenType::INCREMENT:
			Generator::appendOutput("movsd xmm1, [one]");
			Generator::appendOutput("addsd xmm0, xmm1");
			break;
		case TokenType::DECREMENT:
			Generator::appendOutput("movsd xmm1, [one]");
			Generator::appendOutput("subsd xmm0, xmm1");
			break;
		default:
			Error::abortWithLineNumber(op.getName() + " is not handled in increment decrement expression.",
									   op.lineNumber);
			break;
	}

	if (isPrefix && !isStatement) {
		// pop old identifier value out (this value is pushed when calling identifier->generateAssembly) and push updated value in
		Generator::push("xmm0");
	}

	Generator::copyValueToStackFrom("xmm0", variable->getStackOffset());
}