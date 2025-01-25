#include "AST/Scope.h"

#include <iostream>

#include "Generator.h"

void Scope::print(const std::string &indent) const {
	std::cout << indent << "Begin " << getTypeName() << std::endl;

	for (int i = 0; i < statements.size(); i++) {
		statements.at(i)->print("\t");
	}

	std::cout << indent << "End " << getTypeName() << std::endl;
}

void Scope::generateAssembly() const {
	if (isRoot) {
		// initialize stack frame
		Generator::appendComment("Initialize stack frame");
		Generator::appendOutput("push rbp");
		Generator::appendOutput("mov rbp, rsp");
	}

	Generator::startScope();

	for (const std::unique_ptr<Node> &statement : statements) {
		statement->generateAssembly();
	}

	Generator::endScope();
}