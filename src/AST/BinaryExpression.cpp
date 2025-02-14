#include "AST/BinaryExpression.h"

#include <iostream>

#include "AST/Identifier.h"
#include "Error.h"
#include "Generator.h"

void BinaryExpression::print(const std::string& indent) const {
	std::cout << indent << getName() << " (" << getTypeName() << "): " << op.getName() << std::endl;

	left->print(indent + "\t");
	right->print(indent + "\t");
}

void BinaryExpression::generateAssembly() {
	Generator::appendComment("Generating left assembly");
	left->generateAssembly();

	Generator::appendComment("Generating right assembly");
	right->generateAssembly();

	if (left->type != right->type) {
		Error::abortWithLineNumber(
			"Could not perform " + op.value + " with " + left->getTypeName() + " and " + right->getTypeName(),
			op.lineNumber);
	}

	if (left->type == Type::UNKNOWN) {
		Error::abortWithLineNumber("Could not perform " + op.value + " with unknown type", op.lineNumber);
	}

	// now, stack contains the values of left and right
	// pop them off and store them in different registers depending on their data types
	if (left->type == Type::NUMBER) {
		Generator::appendComment("Getting pushed value from right and store in xmm1");
		Generator::pop("xmm1");
		Generator::appendComment("Getting pushed value from left and store in xmm0");
		Generator::pop("xmm0");
	} else if (left->type == Type::BOOLEAN || left->type == Type::STRING) {
		Generator::appendComment("Getting pushed value from right and store in rbx");
		Generator::pop("rbx");
		Generator::appendComment("Getting pushed value from left and store in rax");
		Generator::pop("rax");
	} else {
		Error::abortWithLineNumber("Could not perform " + op.value + " with " + left->getTypeName() + " type",
								   op.lineNumber);
	}

	// results are always stored in xmm0
	switch (op.type) {
		case TokenType::PLUS: {
			Generator::appendComment("Binary Operator: +");
			if (left->type == Type::NUMBER) {
				type = Type::NUMBER;
				Generator::appendOutput("addpd xmm0, xmm1");
				Generator::push("xmm0");
			} else if (left->type == Type::STRING) {
				type = Type::STRING;
				Generator::appendComment(
					"Move rax to r12 because rax will be used for returning values for other c functions. Also can't "
					"use other registers because they are callee-saved");
				Generator::appendOutput("mov r12, rax");

				Generator::appendComment("Calculate length of left string");
				Generator::appendOutput("mov rdi, r12");
				Generator::appendOutput("call strlen");
				Generator::appendOutput("mov r8, rax");	 // strlen stores result in rax

				Generator::appendComment("Calculate length of right string");
				Generator::appendOutput("mov rdi, rbx");
				Generator::appendOutput("call strlen");
				Generator::appendOutput("mov r9, rax");

				Generator::appendComment(
					"Allocate memory using malloc for the sum of both lengths + 1 for the null terminator");
				Generator::appendOutput("add r8, r9");
				Generator::appendOutput("inc r8");
				Generator::appendOutput("mov rdi, r8");
				Generator::appendOutput("call malloc");

				Generator::appendComment("Store the allocated memory pointer in the result");
				Generator::appendOutput("mov rdi, rax");

				Generator::appendComment("Copy string1 into the allocated memory (using strcpy)");
				Generator::appendOutput("mov rsi, r12");
				Generator::appendOutput("call strcpy");

				Generator::appendComment("Copy string2 into the allocated memory (using strcat)");
				Generator::appendOutput("mov rsi, rbx");
				Generator::appendOutput("call strcat");

				Generator::appendComment("At this point, the memory pointed by rax contains the concatenated string.");
				Generator::push("rax");
			} else {
				Error::abortWithLineNumber("Could not perform " + op.value + " on type " + left->getTypeName(),
										   op.lineNumber);
			}
			break;
		}
		case TokenType::MINUS: {
			if (left->type != Type::NUMBER)
				Error::abortWithLineNumber("Could not perform " + op.value + " on type " + left->getTypeName(),
										   op.lineNumber);
			type = Type::NUMBER;
			Generator::appendComment("Binary Operator: -");
			Generator::appendOutput("subpd xmm0, xmm1");
			Generator::push("xmm0");
			break;
		}
		case TokenType::MULTIPLY: {
			if (left->type != Type::NUMBER)
				Error::abortWithLineNumber("Could not perform " + op.value + " on type " + left->getTypeName(),
										   op.lineNumber);
			type = Type::NUMBER;
			Generator::appendComment("Binary Operator: *");
			Generator::appendOutput("mulpd xmm0, xmm1");
			Generator::push("xmm0");
			break;
		}
		case TokenType::DIVIDE: {
			if (left->type != Type::NUMBER)
				Error::abortWithLineNumber("Could not perform " + op.value + " on type " + left->getTypeName(),
										   op.lineNumber);
			type = Type::NUMBER;
			Generator::appendComment("Binary Operator: /");
			Generator::appendOutput("divpd xmm0, xmm1");
			Generator::push("xmm0");
			break;
		}
		case TokenType::DOUBLE_EQUALS: {
			if (left->type != Type::NUMBER && left->type != Type::BOOLEAN && left->type != Type::STRING)
				Error::abortWithLineNumber("Could not perform " + op.value + " on type" + left->getTypeName(),
										   op.lineNumber);
			type = Type::BOOLEAN;
			/*
				_start:
					; Compare rax and rbx
					cmp rax, rbx       ; Compare the values in rax and rbx
					je  .equal          ; Jump to .equal if rax == rbx (JE stands for "jump if equal")
					
					; If not equal, push 0
					push 0
					jmp .done           ; Skip the next part if not equal
					
				.equal:
					; If equal, push 1
					push 1
					
				.done:
					; You can continue here with other code
					; For example, exit or other operations
				*/
			Generator::appendComment("Binary Operator: ==");
			Generator::incrementStack();
			if (left->type == Type::BOOLEAN) Generator::appendOutput("cmp rax, rbx");
			else if (left->type == Type::NUMBER) Generator::appendOutput("comisd xmm0, xmm1");
			else {
				Generator::appendOutput("mov rdi, rax");
				Generator::appendOutput("mov rsi, rbx");
				Generator::appendOutput("call strcmp");
				// if 0 means same, can also be > 0 or < 0, but that does not matter here
				Generator::appendOutput("cmp rax, 0");
			}

			std::string equalLabel = Generator::createLabel();
			std::string doneLabel = Generator::createLabel();
			Generator::appendOutput("je " + equalLabel);
			Generator::appendOutput("mov QWORD [rsp], 0");
			Generator::appendOutput("jmp " + doneLabel);
			Generator::appendOutput(equalLabel + ":", false);
			Generator::appendOutput("mov QWORD [rsp], 1");
			Generator::appendOutput(doneLabel + ":", false);
			break;
		}
		case TokenType::GREATER_THAN:
		case TokenType::GREATER_THAN_OR_EQUALS_TO:
		case TokenType::LESS_THAN:
		case TokenType::LESS_THAN_OR_EQUALS_TO: {
			if (left->type != Type::NUMBER)
				Error::abortWithLineNumber("Could not perform " + op.value + " on type " + left->getTypeName(),
										   op.lineNumber);
			type = Type::BOOLEAN;
			Generator::appendComment("Binary operator: " + op.value);
			Generator::incrementStack();
			Generator::appendOutput("comisd xmm0, xmm1");
			std::unordered_map<TokenType, std::string> operationAssemblyInstruction = {
				{TokenType::GREATER_THAN, "ja"},
				{TokenType::GREATER_THAN_OR_EQUALS_TO, "jae"},
				{TokenType::LESS_THAN, "jb"},
				{TokenType::LESS_THAN_OR_EQUALS_TO, "jbe"},
			};
			std::string trueLabel = Generator::createLabel();
			std::string doneLabel = Generator::createLabel();
			Generator::appendOutput(operationAssemblyInstruction[op.type] + " " + trueLabel);
			Generator::appendOutput("mov QWORD [rsp], 0");
			Generator::appendOutput("jmp " + doneLabel);
			Generator::appendOutput(trueLabel + ":", false);
			Generator::appendOutput("mov QWORD [rsp], 1");
			Generator::appendOutput(doneLabel + ":", false);
			break;
		}
		case TokenType::OR:
		case TokenType::AND: {
			if (left->type != Type::BOOLEAN)
				Error::abortWithLineNumber("Could not perform " + op.value + " on type " + left->getTypeName(),
										   op.lineNumber);

			type = Type::BOOLEAN;
			std::string instruction = op.type == TokenType::OR ? "or" : "and";
			Generator::appendOutput(instruction + " rax, rbx");
			Generator::push("rax");
			break;
		}
		default:
			Error::abortWithLineNumber("Operator " + op.value + " is not a valid binary operator.", op.lineNumber);
	}

	// if its a statement, no need to store the value in the stack
	if (isStatement) Generator::pop();
}