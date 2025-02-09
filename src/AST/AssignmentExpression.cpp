#include "AST/AssignmentExpression.h"

#include <iostream>

#include "AST/Identifier.h"
#include "Error.h"
#include "Generator.h"

void AssignmentExpression::print(const std::string& indent) const {
	std::cout << indent << getName() << " (" << getTypeName() << "): " << op.getName() << std::endl;

	identifier->print(indent + "\t");
	right->print(indent + "\t");
}

void AssignmentExpression::generateAssembly() {
	// check if identifier exists, if not, means haven't declare and can throw error
	Variable* variable = Generator::getVariable(identifier->token.value);

	if (variable == nullptr)
		Error::abortWithLineNumber(identifier->token.value + " has not been declared before assignment.",
								   identifier->token.lineNumber);

	right->generateAssembly();

	// check for type, can only assign to same type or unknown
	if (variable->type != Type::UNKNOWN && variable->type != right->type)
		Error::abortWithLineNumber(
			"Could not assign " + right->getTypeName() + " to " + Expression::typeToStringMap.at(variable->type),
			identifier->token.lineNumber);

	variable->type = right->type;
	type = right->type;

	Generator::appendComment("Assigning " + variable->name + " at stack position " +
							 std::to_string(variable->stackLocation));

	std::string registerName;

	switch (type) {
		case Type::NUMBER:
			registerName = "xmm0";
			break;
		case Type::BOOLEAN:
			registerName = "rax";
			break;
		default:
			Error::abortWithLineNumber("Could not handle assignment of type " + getTypeName() + " yet", op.lineNumber);
	}

	if (isStatement) Generator::pop(registerName);
	Generator::copyValueToStackFrom(registerName, variable->getStackOffset());
}