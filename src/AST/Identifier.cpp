#include "AST/Identifier.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void Identifier::print(const std::string& indent) const {
	std::cout << indent << getName() << " (" << getTypeName() << "): " << token.value << std::endl;
}

void Identifier::generateAssembly() {
	// check if identifier has already been declared
	Variable* variable = Generator::getVariable(token.value);

	if (variable == nullptr) {
		Error::abortWithLineNumber("Variable " + token.value + " has not been declared.", token.lineNumber);
		return;
	}

	type = variable->type;

	if (isStatement) return;

	// push value to top of stack
	Generator::appendComment("Identifier: " + token.value);
	switch (type) {
		case Type::NUMBER:
			Generator::copyValueFromStackTo("xmm0", variable->getStackOffset());
			Generator::push("xmm0");
			break;
		case Type::BOOLEAN:
		case Type::STRING:
			Generator::copyValueFromStackTo("rax", variable->getStackOffset());
			Generator::push("rax");
			break;
		case Type::UNKNOWN:
			break;
	}
}
