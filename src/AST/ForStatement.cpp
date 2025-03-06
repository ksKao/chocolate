#include "AST/ForStatement.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void ForStatement::print(const std::string& indent) const {
	std::cout << indent << getName() << ": " << std::endl;

	std::cout << indent << "Init Statement: " << std::endl;
	if (initStatement.has_value()) initStatement.value()->print(indent + "\t");

	std::cout << indent << "Condition: " << std::endl;
	if (condition.has_value()) condition.value()->print(indent + "\t");

	std::cout << indent << "Update Statement: " << std::endl;
	if (updateExpression.has_value()) updateExpression.value()->print(indent + "\t");

	scope->print(indent + "\t");
}

void ForStatement::generateAssembly() {
	Generator::appendComment("For statement");

	if (initStatement.has_value()) initStatement.value()->generateAssembly();
	std::string comparisonLabel = Generator::createLabel();
	Generator::appendOutput("jmp " + comparisonLabel);

	std::string loopCodeLabel = Generator::createLabel();
	Generator::appendOutput(loopCodeLabel + ": ", false);

	Generator::startScope();
	scope->generateAssembly();
	if (updateExpression.has_value()) updateExpression.value()->generateAssembly();
	Generator::pop();
	Generator::endScope();

	Generator::appendOutput(comparisonLabel + ": ", false);
	Generator::appendComment("For statement condition");
	if (condition.has_value()) condition.value()->generateAssembly();
	// need to call this after calling condition->generateAssembly to get the type
	if (condition.has_value() && condition.value()->type != Type::BOOLEAN)
		Error::abortWithLineNumber("Expected boolean type for while statement condition, but received " +
									   condition.value()->getTypeName() + " instead",
								   forToken.lineNumber);

	Generator::pop("rax");
	Generator::appendOutput("cmp rax, 1");
	Generator::appendOutput("je " + loopCodeLabel);
}
