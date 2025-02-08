#include "AST/ForStatement.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void ForStatement::print(const std::string& indent) const {
	std::cout << indent << getName() << ": " << std::endl;

	std::cout << indent << "Init Statement: " << std::endl;
	initStatement->print(indent + "\t");

	std::cout << indent << "Condition: " << std::endl;
	condition->print(indent + "\t");

	std::cout << indent << "Update Statement: " << std::endl;
	updateStatement->print(indent + "\t");

	scope->print(indent + "\t");
}

void ForStatement::generateAssembly() {
	Generator::appendComment("For statement");

	// start scope here so that the variables declared inside the init statement will not spill over
	Generator::startScope();

	initStatement->generateAssembly();
	std::string comparisonLabel = Generator::createLabel();
	Generator::appendOutput("jmp " + comparisonLabel);

	std::string loopCodeLabel = Generator::createLabel();
	Generator::appendOutput(loopCodeLabel + ": ", false);

	Generator::startScope();
	scope->generateAssembly();
	updateStatement->generateAssembly();
	Generator::endScope();

	Generator::appendOutput(comparisonLabel + ": ", false);
	Generator::appendComment("For statement condition");
	condition->generateAssembly();
	// need to call this after calling condition->generateAssembly to get the type
	if (condition->type != Type::BOOLEAN)
		Error::abortWithLineNumber("Expected boolean type for while statement condition, but received " +
									   condition->getTypeName() + " instead",
								   forToken.lineNumber);

	Generator::pop("rax");
	Generator::appendOutput("cmp rax, 1");
	Generator::appendOutput("je " + loopCodeLabel);

	Generator::endScope();
}