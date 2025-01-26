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
	Generator::incrementStack();
	Generator::appendComment("Identifier: " + name);
	if (type == Type::NUMBER) {
		Generator::appendOutput("movsd xmm0, QWORD [rsp + " +
								std::to_string(variable->getStackOffset()) + "]");
		Generator::appendOutput("movsd QWORD [rsp], xmm0");
	} else if (type == Type::BOOLEAN) {
		Generator::appendOutput("mov rax, QWORD [rsp + " +
								std::to_string(variable->getStackOffset()) + "]");
		Generator::appendOutput("mov QWORD [rsp], rax");
	} else {
	}
}