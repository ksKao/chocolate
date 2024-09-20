#include "AST/Identifier.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void Identifier::print(const std::string& indent) const {
	std::cout << indent << getTypeName() << ": " << name << std::endl;
}

void Identifier::generateAssembly() const {
	// check if identifier has already been declared
	std::optional<Variable> variable = Generator::getVariable(name);

	if (!variable.has_value()) {
		exitWithError("Variable " + name + " has not been declared.");
		return;
	}

	size_t stackOffset =
		(Generator::getStackSize() - variable.value().stackLocation) * Generator::stackUnitSize;

	Generator::appendOutput("movsd xmm0, QWORD [rsp + " + std::to_string(stackOffset) + "]");
}