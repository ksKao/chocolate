#include "AST/PrintStatement.h"

#include <iostream>

#include "Error.h"
#include "Generator.h"

void PrintStatement::print(const std::string& indent) const {
	std::cout << indent << getName() << std::endl;

	value->print(indent + "\t");
}

void PrintStatement::generateAssembly() {
	value->generateAssembly();

	Generator::appendComment("Print Statement");

	switch (value->type) {
		case Type::NUMBER: {
			Generator::appendOutput("movsd xmm0, QWORD [rsp]");
			Generator::appendOutput("mov rdi, float_format");
			break;
		}
		case Type::BOOLEAN: {
			std::string equalLabel = Generator::createLabel();
			std::string doneLabel = Generator::createLabel();

			Generator::appendOutput("mov rdi, string_format");
			Generator::appendOutput("mov rax, QWORD [rsp]");
			Generator::appendOutput("cmp rax, 0");
			Generator::appendOutput("je " + equalLabel);
			Generator::appendOutput("mov rsi, true");
			Generator::appendOutput("jmp " + doneLabel);
			Generator::appendOutput(equalLabel + ":", false);
			Generator::appendOutput("mov rsi, false");
			Generator::appendOutput(doneLabel + ":", false);
			break;
		}
		default: {
			Error::abort("Could not print value with " + value->getTypeName() + " type.");
			break;
		}
	}

	Generator::appendOutput("mov rax, 1");
	Generator::appendOutput("call printf");
	Generator::decrementStack();
}