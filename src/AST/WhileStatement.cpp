#include "AST/WhileStatement.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void WhileStatement::print(const std::string& indent) const {
	std::cout << indent << getName() << ": " << std::endl;
	std::cout << indent << "Condition: " << std::endl;

	condition->print(indent + "\t");

	scope->print(indent + "\t");
}

void WhileStatement::generateAssembly() {
	Generator::appendComment("While statement");
	std::string comparisonLabel = Generator::createLabel();
	Generator::appendOutput("jmp " + comparisonLabel);

	std::string loopCodeLabel = Generator::createLabel();
	Generator::appendOutput(loopCodeLabel + ": ", false);
	Generator::startScope();
	scope->generateAssembly();
	Generator::endScope();

	Generator::appendOutput(comparisonLabel + ": ", false);
	Generator::appendComment("While statement condition: ");

	condition->generateAssembly();
	// need to call this after calling condition->generateAssembly to get the type
	if (condition->type != Type::BOOLEAN)
		Error::abortWithLineNumber("Expected boolean type for while statement condition, but received " +
									   condition->getTypeName() + " instead",
								   whileToken.lineNumber);

	Generator::pop("rax");
	Generator::appendOutput("cmp rax, 1");
	Generator::appendOutput("je " + loopCodeLabel);
}