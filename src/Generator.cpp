#include "Generator.h"

#include <iostream>
#include <sstream>

#include "AST/Expression.h"
#include "Error.h"

size_t Variable::getStackOffset() {
	return (Generator::getStackSize() - stackLocation - 1) * Generator::stackUnitSize;
}

void Generator::appendOutput(const std::string &line, bool indent) {
	output->push_back({line, indent});
}

void Generator::appendComment(const std::string &line) {
	output->push_back({"; " + line, true});
}

std::stringstream Generator::getOutput(Scope &program) {
	// headers
	Generator::appendOutput("section .text", false);
	Generator::appendOutput("global main");
	Generator::appendOutput("default rel");
	Generator::appendOutput("extern printf");
	Generator::appendOutput("");

	Generator::appendOutput("main:", false);
	program.generateAssembly();

	// return 0
	Generator::appendComment("return 0");
	Generator::appendOutput("mov eax, 60");
	Generator::appendOutput("xor edi, edi");
	Generator::appendOutput("syscall");

	std::stringstream outputString;

	outputString << "section .data" << std::endl;
	for (size_t i = 0; i < Generator::data.size(); i++)
		outputString << "\td" << i << ' ' << data.at(i).size << ' ' << data.at(i).value
					 << std::endl;

	// add an entry for null
	outputString << "\tnull DQ 0" << std::endl;
	// add another entry for float printf format
	outputString << "\tfloat_format db `%f\\n`" << std::endl;
	outputString << std::endl;

	for (const OutputLine &outputLine : *output)
		outputString << (outputLine.indent ? "\t" : "") << outputLine.content << std::endl;

	return outputString;
}

std::string Generator::getDataName(const std::string &value, const std::string &size) {
	for (size_t i = 0; i < data.size(); i++) {
		if (data.at(i).value == value && data.at(i).size == size) return "d" + std::to_string(i);
	}

	// if reach here means the data has not been declared in the data section
	data.push_back({size, value});

	return "d" + std::to_string(data.size() - 1);
}

size_t Generator::getStackSize() {
	return stackSize;
}

void Generator::addVariable(const std::string &variableName, Type type) {
	if (Generator::getVariable(variableName) != nullptr)
		Error::abort("Trying to add a variable (" + variableName + ") when it already exists: ");

	variables.emplace_back(variableName, stackSize - 1, type);
}

Variable *Generator::getVariable(const std::string variableName) {
	for (Variable &variable : variables) {
		if (variable.name == variableName) return &variable;
	}

	return nullptr;
}

std::string Generator::createLabel() {
	labelCounter++;
	return "label" + std::to_string(labelCounter);
}

void Generator::startScope() {
	numbersOfVariablesDeclaredBeforeScope.push_back(variables.size());
}

void Generator::endScope() {
	size_t popCount = variables.size() - numbersOfVariablesDeclaredBeforeScope.back();

	stackSize -= popCount;

	Generator::appendComment("Pop scope");
	Generator::appendOutput("add rsp, " + std::to_string(popCount * Generator::stackUnitSize));

	variables.resize(variables.size() - popCount);

	numbersOfVariablesDeclaredBeforeScope.pop_back();
}

void Generator::incrementStack() {
	Generator::appendComment("Increment stack");
	Generator::appendOutput("sub rsp, " +
							std::to_string(Generator::stackUnitSize));	// move top stack pointer up
	stackSize++;
}

void Generator::decrementStack() {
	Generator::appendComment("Decrement stack");
	Generator::appendOutput(
		"add rsp, " + std::to_string(Generator::stackUnitSize));  // move top stack pointer down
	stackSize--;
}

size_t Generator::stackSize = 0;
size_t Generator::labelCounter = 0;
std::unique_ptr<std::vector<OutputLine>> Generator::output =
	std::make_unique<std::vector<OutputLine>>();

// need these lines otherwise will have linking error
std::vector<Variable> Generator::variables;
std::vector<size_t> Generator::numbersOfVariablesDeclaredBeforeScope;
std::vector<Data> Generator::data;