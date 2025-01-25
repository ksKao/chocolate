#include "AST/AssignmentStatement.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void AssignmentStatement::print(const std::string& indent) const {
	std::cout << indent << getTypeName() << ": " << std::endl;

	rhs->print(indent + "\t");
}

void AssignmentStatement::generateAssembly() const {
	// check if identifier exists, if not, means haven't declare and can throw error
	std::optional<Variable> variable = Generator::getVariable(identifier.value);

	if (!variable.has_value())
		Error::abort(identifier.value + " has not been declared before assignment.");

	rhs->generateAssembly();

	Generator::appendComment("Assigning " + variable->name + " at stack position " +
							 std::to_string(variable->stackLocation));
	Generator::appendOutput("movsd xmm0, QWORD [rsp]");
	Generator::appendOutput("movsd QWORD [rsp + " + std::to_string(variable->getStackOffset()) +
							"], xmm0");

	Generator::decrementStack();
}
