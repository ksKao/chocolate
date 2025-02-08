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
	} else if (right->type == Type::BOOLEAN) {
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
			if (left->type != Type::NUMBER)
				Error::abortWithLineNumber("Could not perform " + op.value + " on type " + left->getTypeName(),
										   op.lineNumber);
			type = Type::NUMBER;
			Generator::appendComment("Binary Operator: +");
			Generator::appendOutput("addpd xmm0, xmm1");
			Generator::push("xmm0");
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
			if (left->type != Type::NUMBER && left->type != Type::BOOLEAN)
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
			else Generator::appendOutput("comisd xmm0, xmm1");

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
		case TokenType::EQUALS: {
			Identifier* identifier = dynamic_cast<Identifier*>(left.get());

			if (identifier == nullptr)
				Error::abortWithLineNumber("Left side of assignment operator is not an identifier.", op.lineNumber);

			// check if identifier exists, if not, means haven't declare and can throw error
			Variable* variable = Generator::getVariable(identifier->token.value);

			if (variable == nullptr)
				Error::abortWithLineNumber(identifier->token.value + " has not been declared before assignment.",
										   identifier->token.lineNumber);

			right->generateAssembly();

			// check for type, can only assign to same type or unknown
			if (variable->type != Type::UNKNOWN && variable->type != right->type)
				Error::abortWithLineNumber("Could not assign " + right->getTypeName() + " to " +
											   Expression::typeToStringMap.at(variable->type),
										   identifier->token.lineNumber);

			variable->type = right->type;
			type = right->type;

			Generator::appendComment("Assigning " + variable->name + " at stack position " +
									 std::to_string(variable->stackLocation));

			std::string registerName;

			switch (type) {
				case Type::NUMBER:
					registerName = "xmm0";
					break;
				case Type::BOOLEAN:
					registerName = "rax";
					break;
				default:
					Error::abortWithLineNumber("Could not handle assignment of type " + getTypeName() + " yet",
											   op.lineNumber);
			}

			Generator::pop(registerName);
			Generator::copyValueToStackFrom(registerName, variable->getStackOffset());

			// push this because all expressions should end with pushing something to the stack
			Generator::push(registerName);
			break;
		}
		default:
			Error::abortWithLineNumber("Operator " + op.value + " is not a valid binary operator.", op.lineNumber);
	}

	// if its a statement, no need to store the value in the stack
	if (isStatement) Generator::pop();
}