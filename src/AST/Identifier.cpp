#include "AST/Identifier.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void Identifier::print(const std::string& indent) const {
	std::cout << indent << getName() << " (" << getTypeName() << "): " << name << std::endl;
}

void Identifier::generateAssembly() {
	// check if identifier has already been declared
	Variable* variable = Generator::getVariable(name);

	if (variable == nullptr) {
		Error::abort("Variable " + name + " has not been declared.");
		return;
	}

	type = variable->type;

	// push value to top of stack
	Generator::appendComment("Identifier: " + name);
	if (type == Type::NUMBER) {
		Generator::copyValueFromStackTo("xmm0", variable->getStackOffset());
		Generator::push("xmm0");
	} else if (type == Type::BOOLEAN) {
		Generator::copyValueFromStackTo("rax", variable->getStackOffset());
		Generator::push("rax");
	} else if (type != Type::UNKNOWN) {
		Error::abort("Could not handle identifier of type " + getTypeName());
	}
}