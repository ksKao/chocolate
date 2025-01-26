#include "AST/IfStatement.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void IfStatement::print(const std::string& indent) const {
	std::cout << indent << getName() << ": " << std::endl;
	std::cout << indent << "Condition: ";

	condition->print("");

	scope->print(indent + "\t");
}

void IfStatement::generateAssembly() {
	Generator::appendComment("If statement condition");
	condition->generateAssembly();

	if (condition->type != Type::BOOLEAN)
		Error::abort("Expected boolean type for if statement condition, but received " +
					 condition->getTypeName() + " instead");

	std::string label = Generator::createLabel();

	Generator::appendComment("If statement");
	Generator::appendOutput("mov rax, QWORD [rsp]");

	Generator::decrementStack();

	Generator::appendOutput("cmp rax, 0");
	Generator::appendOutput("je " + label);

	scope->generateAssembly();
	Generator::appendOutput(label + ":", false);
}