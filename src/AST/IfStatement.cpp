#include "AST/IfStatement.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void IfStatement::print(const std::string& indent) const {
	std::cout << indent << getName() << ": " << std::endl;
	std::cout << indent << "Condition: " << std::endl;

	condition->print(indent + "\t");

	scope->print(indent + "\t");

	if (!alternate.has_value()) return;

	if (const std::unique_ptr<Scope>* scope = std::get_if<std::unique_ptr<Scope>>(&alternate.value())) {
		std::cout << indent << "Else: " << std::endl;
		(**scope).print(indent + "\t");
	} else if (const std::unique_ptr<IfStatement>* ifStatement =
				   std::get_if<std::unique_ptr<IfStatement>>(&alternate.value())) {
		std::cout << indent << "Else If: " << std::endl;
		(**ifStatement).print(indent + "\t");
	} else {
		Error::abortWithLineNumber("Could not print invalid else node type.", ifToken.lineNumber);
	}
}

void IfStatement::generateAssembly() {
	Generator::appendComment("If statement condition");
	condition->generateAssembly();

	if (condition->type != Type::BOOLEAN)
		// line number may not be accurate since the if token and the condition may not be on the same line, but this should be good enough
		Error::abortWithLineNumber(
			"Expected boolean type for if statement condition, but received " + condition->getTypeName() + " instead",
			ifToken.lineNumber);

	bool isFirstIf = doneLabel == "";
	if (isFirstIf) doneLabel = Generator::createLabel();

	Generator::appendComment("If statement");
	Generator::pop("rax");

	Generator::appendOutput("cmp rax, 0");

	if (!alternate.has_value()) {
		Generator::appendOutput("je " + doneLabel);
		scope->generateAssembly();
	} else {
		std::string elseLabel = Generator::createLabel();
		Generator::appendComment("else if");
		Generator::appendOutput("je " + elseLabel);
		scope->generateAssembly();
		Generator::appendOutput("jmp " + doneLabel);
		if (alternate.has_value()) Generator::appendOutput(elseLabel + ": ", false);

		if (const std::unique_ptr<IfStatement>* ifStatement =
				std::get_if<std::unique_ptr<IfStatement>>(&alternate.value())) {
			(*ifStatement)->doneLabel = doneLabel;
			(*ifStatement)->generateAssembly();
		} else if (const std::unique_ptr<Scope>* elseScope = std::get_if<std::unique_ptr<Scope>>(&alternate.value())) {
			Generator::appendComment("else");
			(*elseScope)->generateAssembly();
		} else {
			Error::abortWithLineNumber("Invalid data type for if statement alternate.", ifToken.lineNumber);
		}
	}

	if (isFirstIf) Generator::appendOutput(doneLabel + ":", false);
}