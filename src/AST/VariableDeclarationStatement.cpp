#include "AST/VariableDeclarationStatement.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void VariableDeclarationStatement::print(const std::string &indent) const {
	std::cout << indent << getName() << ": name = " << identifier->name << std::endl;

	value->print(indent + "\t");
}

void VariableDeclarationStatement::generateAssembly() {
	// check if variable has already been declared
	if (Generator::getVariable(identifier->name) != nullptr)
		Error::abort("Identifier " + identifier->name + " already exists.");

	value->generateAssembly();

	// move the value stored in the top of the stack
	Generator::appendComment(
		"Variable Declaration Statement (not doing anything since the right side will already be "
		"pushing a value onto the stack): " +
		identifier->name);
	Generator::addVariable(identifier->name, value->type);
}
