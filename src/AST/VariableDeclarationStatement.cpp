#include "AST/VariableDeclarationStatement.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void VariableDeclarationStatement::print(const std::string &indent) const {
	std::cout << indent << getTypeName() << ": name = " << identifier.value << std::endl;

	value->print(indent + "\t");
}

void VariableDeclarationStatement::generateAssembly() const {
	// check if variable has already been declared
	if (Generator::getVariable(identifier.value).has_value())
		exitWithError("Identifier " + identifier.value + " already exists.");

	value->generateAssembly();
	Generator::incrementStack();
	Generator::addVariable(identifier.value);
	// move the value stored in xmm into the top stack pointer
	Generator::appendOutput("movsd QWORD [rsp], xmm0");
}
