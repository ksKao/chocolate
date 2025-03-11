#include "AST/VariableDeclarationStatement.h"

#include <iostream>

#include "AST/Identifier.h"
#include "Error.h"
#include "Generator.h"

void VariableDeclarationStatement::print(const std::string &indent) const {
	std::cout << indent << getName() << ": name = " << identifier->token.value << std::endl;

	value->print(indent + "\t");
}

void VariableDeclarationStatement::generateAssembly() {
	// check if variable has already been declared
	if (Generator::getVariable(identifier->token.value) != nullptr)
		Error::abortWithLineNumber("Identifier " + identifier->token.value + " already exists.",
								   identifier->token.lineNumber);

	value->generateAssembly();

	// move the value stored in the top of the stack
	Generator::appendComment(
		"Variable Declaration Statement (not doing anything since the right side will already be "
		"pushing a value onto the stack): " +
		identifier->token.value);
	Generator::addVariable(identifier->token.value, value->type, identifier->token.lineNumber);
}
