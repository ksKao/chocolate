#include "AST/BinaryExpression.h"

#include <iostream>
#include <algorithm>

#include "AST/BinaryExpression.h"
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

	// now, stack contains the values of left and right
	// pop them off and store them in different registers depending on their data types
	if (right->type == Type::NUMBER) {
		Generator::appendComment("Getting pushed value from right and store in xmm1");
		Generator::pop("xmm1");
	} else if (right->type == Type::BOOLEAN || right->type == Type::STRING) {
		Generator::appendComment("Getting pushed value from right and store in rbx");
		Generator::pop("rbx");
	}

	if (left->type == Type::NUMBER) {
		Generator::appendComment("Getting pushed value from left and store in xmm0");
		Generator::pop("xmm0");
	} else if (left->type == Type::BOOLEAN || left->type == Type::STRING) {
		Generator::appendComment("Getting pushed value from left and store in rax");
		Generator::pop("rax");
	}

	switch (op.type) {
		case TokenType::PLUS:
			handlePlus();
			break;
		case TokenType::MINUS:
			handleMinus();
			break;
		case TokenType::MULTIPLY:
			handleMultiply();
			break;
		case TokenType::DIVIDE:
			handleDivide();
			break;

		case TokenType::DOUBLE_EQUALS:
			handleDoubleEquals();
			break;
		case TokenType::GREATER_THAN:
		case TokenType::GREATER_THAN_OR_EQUALS_TO:
		case TokenType::LESS_THAN:
		case TokenType::LESS_THAN_OR_EQUALS_TO:
			handleNumberComparison();
			break;
		case TokenType::OR:
		case TokenType::AND:
			handleBoolean();
			break;
		default:
			Error::abortWithLineNumber("Operator " + op.value + " is not a valid binary operator.", op.lineNumber);
	}

	// if its a statement, no need to store the value in the stack
	if (isStatement) Generator::pop();
}

void BinaryExpression::abortWithTypeError() {
	Error::abortWithLineNumber(
		"Could not perform " + op.value + " with " + left->getTypeName() + " and " + right->getTypeName(),
		op.lineNumber);
}

void BinaryExpression::handlePlus() {
	Generator::appendComment("Binary Operator: +");

	// if either left or right is string, do string concatenation
	if (left->type == Type::STRING || right->type == Type::STRING) {
		type = Type::STRING;

		// need to move to r12 because other registers are callee-saved and rax is used to return values by other c functions
		if (left->type == Type::BOOLEAN || right->type == Type::BOOLEAN) {
			std::string equalLabel = Generator::createLabel();
			std::string doneLabel = Generator::createLabel();

			std::string stringRegister;

			if (left->type == Type::BOOLEAN) {
				Generator::appendOutput("cmp rax, 0");
				stringRegister = "r12";
			} else {
				Generator::appendOutput("cmp rbx, 0");
				Generator::appendOutput("mov r12, rax");
				stringRegister = "rbx";
			}

			Generator::appendOutput("je " + equalLabel);
			Generator::appendOutput("mov " + stringRegister + ", true");
			Generator::appendOutput("jmp " + doneLabel);
			Generator::appendOutput(equalLabel + ":", false);
			Generator::appendOutput("mov " + stringRegister + ", false");
			Generator::appendOutput(doneLabel + ":", false);
		} else if (left->type == Type::NUMBER || right->type == Type::NUMBER) {
			std::string stringRegister;
			if (right->type == Type::NUMBER) {
				Generator::appendOutput("movsd xmm0, xmm1");
				Generator::appendOutput("mov r12, rax");
				stringRegister = "rbx";
			} else {
				stringRegister = "r12";
			}

			Generator::push("xmm0");  // need to pre-push xmm0 to the stack because malloc will clobber this register
			Generator::appendOutput("mov rdi, 15");	 // allocate 15 chars for the string
			Generator::appendOutput("call malloc");
			Generator::appendOutput("mov " + stringRegister + ", rax");
			Generator::pop("xmm0");

			Generator::appendOutput("mov rdi, " + stringRegister);
			Generator::appendOutput("mov rsi, 15");
			Generator::appendOutput("mov rdx, float_format");
			Generator::appendOutput("mov rax, 1");
			Generator::appendOutput("call snprintf");
		} else if (left->type == Type::UNKNOWN || right->type == Type::UNKNOWN) {
			if (right->type == Type::UNKNOWN) {
				Generator::appendOutput("mov r12, rax");
				Generator::appendOutput("mov rbx, unknown");
			} else {
				Generator::appendOutput("mov r12, unknown");
			}
		} else {
			Generator::appendOutput("mov r12, rax");
		}

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
		Generator::appendOutput("mov rdi, rax");
		Generator::appendOutput("mov rsi, rbx");
		Generator::appendOutput("call strcat");

		Generator::appendComment("At this point, the memory pointed by rax contains the concatenated string.");
		Generator::push("rax");
	} else if (left->type == Type::NUMBER && right->type == Type::NUMBER) {
		type = Type::NUMBER;
		Generator::appendOutput("addpd xmm0, xmm1");
		Generator::push("xmm0");
	} else {
		Error::abortWithLineNumber("Could not perform " + op.value + " on type " + left->getTypeName(), op.lineNumber);
	}
}

void BinaryExpression::handleMinus() {
	if (left->type != Type::NUMBER || right->type != Type::NUMBER) abortWithTypeError();
	type = Type::NUMBER;
	Generator::appendComment("Binary Operator: -");
	Generator::appendOutput("subpd xmm0, xmm1");
	Generator::push("xmm0");
}

void BinaryExpression::handleMultiply() {
	if (left->type != Type::NUMBER || right->type != Type::NUMBER) abortWithTypeError();
	type = Type::NUMBER;
	Generator::appendComment("Binary Operator: *");
	Generator::appendOutput("mulpd xmm0, xmm1");
	Generator::push("xmm0");
}

void BinaryExpression::handleDivide() {
	if (left->type != Type::NUMBER || right->type != Type::NUMBER) abortWithTypeError();
	type = Type::NUMBER;
	Generator::appendComment("Binary Operator: /");
	Generator::appendOutput("divpd xmm0, xmm1");
	Generator::push("xmm0");
}

void BinaryExpression::handleDoubleEquals() {
	if (left->type != right->type) abortWithTypeError();

	Type validTypes[] = {Type::NUMBER, Type::BOOLEAN, Type::STRING};
	Expression* leftTemp = left.get();
	bool isValidComparisonType = std::find_if(std::begin(validTypes), std::end(validTypes), [leftTemp](Type t) {
									 return leftTemp->type == t;
								 }) != std::end(validTypes);

	if (!isValidComparisonType) abortWithTypeError();

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
}

void BinaryExpression::handleNumberComparison() {
	if (left->type != Type::NUMBER || right->type != Type::NUMBER) abortWithTypeError();
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
}

void BinaryExpression::handleBoolean() {
	if (left->type != Type::BOOLEAN || right->type != Type::BOOLEAN) abortWithTypeError();

	type = Type::BOOLEAN;
	std::string instruction = op.type == TokenType::OR ? "or" : "and";
	Generator::appendOutput(instruction + " rax, rbx");
	Generator::push("rax");
}
