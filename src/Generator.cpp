#include "Generator.h"

#include <iostream>
#include <sstream>

#include "Error.h"

void Generator::appendOutput(const std::string &line, bool indent) {
	output->push_back({line, indent});
}

std::string Generator::getOutput(const Program &program) {
	// headers
	Generator::appendOutput("section .text", false);
	Generator::appendOutput("global _start");
	Generator::appendOutput("");

	Generator::appendOutput("_start:", false);
	program.generateAssembly();

	// return 0
	Generator::appendOutput("mov eax, 60");
	Generator::appendOutput("xor edi, edi");
	Generator::appendOutput("syscall");

	std::stringstream outputString;

	outputString << "section .data" << std::endl;
	for (size_t i = 0; i < Generator::data.size(); i++)
		outputString << "\td" << i << ' ' << data.at(i).size << ' ' << data.at(i).value
					 << std::endl;

	// add an entry for null
	outputString << "\tnull DD 0" << std::endl;
	outputString << std::endl;

	for (const OutputLine &outputLine : *output)
		outputString << (outputLine.indent ? "\t" : "") << outputLine.content << std::endl;

	return outputString.str();
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

void Generator::addVariable(const std::string &variableName) {
	if (Generator::getVariable(variableName).has_value())
		exitWithError("Trying to add a variable (" + variableName + ") when it already exists: ");

	variables.insert({variableName, stackSize});
}

std::optional<Variable> Generator::getVariable(const std::string variableName) {
	if (variables.find(variableName) == variables.end()) {
		return std::nullopt;
	}

	return Variable{variableName, variables[variableName]};
}

void Generator::incrementStack() {
	Generator::appendOutput("sub rsp, " +
							std::to_string(Generator::stackUnitSize));	// move top stack pointer up
	stackSize++;
}

void Generator::decrementStack() {
	Generator::appendOutput(
		"add rsp, " + std::to_string(Generator::stackUnitSize));  // move top stack pointer down
	stackSize--;
}

size_t Generator::stackSize = 0;
std::unique_ptr<std::vector<OutputLine>> Generator::output =
	std::make_unique<std::vector<OutputLine>>();
std::unordered_map<std::string, size_t> Generator::variables;
std::vector<Data> Generator::data;