#include "AST/AssignmentStatement.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void AssignmentStatement::print(const std::string& indent) const {
	std::cout << indent << getName() << ": " << std::endl;

	rhs->print(indent + "\t");
}

void AssignmentStatement::generateAssembly() {
	// check if identifier exists, if not, means haven't declare and can throw error
	Variable* variable = Generator::getVariable(identifier->name);

	if (variable == nullptr) Error::abort(identifier->name + " has not been declared before assignment.");

	rhs->generateAssembly();

	// check for type, can only assign to same type or unknown
	if (variable->type != Type::UNKNOWN && variable->type != rhs->type)
		Error::abort("Could not assign " + rhs->getTypeName() + " to " +
					 Expression::typeToStringMap.at(variable->type));

	variable->type = rhs->type;

	Generator::appendComment("Assigning " + variable->name + " at stack position " +
							 std::to_string(variable->stackLocation));
	Generator::pop("xmm0");
	Generator::copyValueToStackFrom("xmm0", variable->getStackOffset());
}
