#include "AST/Scope.h"

#include <iostream>

#include "Generator.h"

void Scope::print(const std::string &indent) const {
	std::cout << indent << "Begin " << getName() << std::endl;

	for (int i = 0; i < statements.size(); i++) {
		statements.at(i)->print(indent + "\t");
	}

	std::cout << indent << "End " << getName() << std::endl;
}

void Scope::generateAssembly() {
	if (isRoot) {
		// initialize stack frame
		Generator::appendComment("Initialize stack frame");
		Generator::appendOutput("push rbp");
		Generator::appendOutput("mov rbp, rsp");
	}

	Generator::startScope();

	for (std::unique_ptr<Node> &statement : statements) {
		statement->generateAssembly();
	}

	Generator::endScope();
}