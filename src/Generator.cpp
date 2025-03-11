#include "Generator.h"

#include <iostream>
#include <sstream>

#include "AST/Expression.h"
#include "Error.h"

size_t Variable::getStackOffset() {
	return (Generator::getStackSize() - stackLocation - 1) * Generator::stackUnitSize;
}

void Generator::appendOutput(const std::string &line, bool indent) {
	if (functionTrace.size() == 0) output->push_back({line, indent});
	else functionTrace.back()->assemblyDefinition->push_back({line, indent});
}

void Generator::appendComment(const std::string &line) {
	if (functionTrace.size() == 0) output->push_back({"; " + line, true});
	else functionTrace.back()->assemblyDefinition->push_back({"; " + line, true});
}

std::stringstream Generator::getOutput(Scope &program) {
	// headers
	appendOutput("section .text", false);
	appendOutput("global main");
	appendOutput("default rel");
	appendOutput("extern printf, strlen, strcat, strcpy, strcmp, snprintf, malloc");
	appendOutput("");

	appendOutput("main:", false);
	program.generateAssembly();

	// return 0
	appendComment("return 0");
	appendOutput("mov eax, 60");
	appendOutput("xor edi, edi");
	appendOutput("syscall");

	std::stringstream outputString;

	outputString << "section .data" << std::endl;
	for (size_t i = 0; i < data.size(); i++)
		outputString << "\td" << i << ' ' << data.at(i).size << ' ' << data.at(i).value << std::endl;

	// add an entry for null
	outputString << "\tnull DQ 0.0" << std::endl;

	// add an entry for 1 (used for increment/decrement)
	outputString << "\tone DQ 1.0" << std::endl;

	// setup for printf
	outputString << "\tfloat_format_endl db `%f\\n`, 0" << std::endl;
	outputString << "\tfloat_format db `%f`, 0" << std::endl;
	outputString << "\tstring_format db `%s\\n`, 0" << std::endl;
	outputString << "\ttrue db 'true', 0" << std::endl;
	outputString << "\tfalse db 'false', 0" << std::endl;
	outputString << "\tunknown db 'unknown', 0" << std::endl;

	outputString << std::endl;

	for (const OutputLine &outputLine : *output)
		outputString << (outputLine.indent ? "\t" : "") << outputLine.content << std::endl;

	for (const std::shared_ptr<Function> function : allFunctions) {
		outputString << function->label << ":" << std::endl;
		size_t test = function->assemblyDefinition->size();
		for (const OutputLine &outputLine : *function->assemblyDefinition) {
			outputString << (outputLine.indent ? "\t" : "") << outputLine.content << std::endl;
		}
		outputString << std::endl;
	}

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

void Generator::addVariable(const std::string &variableName, Type type, size_t lineNo) {
	if (getVariable(variableName) != nullptr)
		Error::abortWithLineNumber("Trying to add a variable (" + variableName + ") when it already exists. ", lineNo);

	variables.emplace_back(variableName, stackSize - 1, type);
}

Variable *Generator::getVariable(const std::string variableName) {
	for (Variable &variable : variables) {
		if (variable.name == variableName) return &variable;
	}

	return nullptr;
}

void Generator::addFunctions(const std::vector<std::unique_ptr<FunctionDeclarationStatement>> &functionsToAdd) {
	functions.reserve(functionsToAdd.size());
	allFunctions.reserve(functionsToAdd.size());

	for (const std::unique_ptr<FunctionDeclarationStatement> &functionToAdd : functionsToAdd) {
		if (getFunction(functionToAdd->identifier->token.value) != nullptr)
			Error::abortWithLineNumber("Trying to declare a function (" + functionToAdd->identifier->token.value +
										   ") when it already exists. ",
									   functionToAdd->functionToken.lineNumber);

		functions.emplace_back(new Function{functionToAdd->identifier->token.value, createLabel()});

		allFunctions.push_back(functions.back());
	}
}

std::shared_ptr<Function> Generator::getFunction(const std::string functionName) {
	for (const std::shared_ptr<Function> function : functions) {
		if (function->name == functionName) return function;
	}

	return nullptr;
}

std::string Generator::createLabel() {
	labelCounter++;
	return "label" + std::to_string(labelCounter);
}

void Generator::startScope() {
	numbersOfVariablesDeclaredBeforeScope.push_back(variables.size());
	numbersOfFunctionsDeclaredBeforeScope.push_back(functions.size());
}

void Generator::endScope() {
	size_t variablePopCount = variables.size() - numbersOfVariablesDeclaredBeforeScope.back();
	size_t functionPopCount = functions.size() - numbersOfFunctionsDeclaredBeforeScope.back();

	stackSize -= variablePopCount;

	appendComment("Pop scope");
	appendOutput("add rsp, " + std::to_string(variablePopCount * stackUnitSize));

	variables.resize(variables.size() - variablePopCount);
	functions.resize(functions.size() - functionPopCount);

	numbersOfVariablesDeclaredBeforeScope.pop_back();
	numbersOfFunctionsDeclaredBeforeScope.pop_back();
}

void Generator::incrementStack() {
	appendComment("Increment stack");
	appendOutput("sub rsp, " + std::to_string(stackUnitSize));	// move top stack pointer up
	stackSize++;
}

void Generator::decrementStack() {
	appendComment("Decrement stack");
	appendOutput("add rsp, " + std::to_string(stackUnitSize));	// move top stack pointer down
	stackSize--;
}

void Generator::copyValueFromStackTo(const std::string &reg, int offset) {
	std::string instruction = reg.starts_with("xmm") ? "movq" : "mov";
	if (offset == 0) {
		appendOutput(instruction + " " + reg + ", [rsp]");
	} else {
		appendOutput(instruction + " " + reg + ", [rsp + " + std::to_string(offset) + "]");
	}
}

void Generator::copyValueToStackFrom(const std::string &reg, int offset) {
	std::string instruction = reg.starts_with("xmm") ? "movq" : "mov";
	if (offset == 0) {
		appendOutput(instruction + " QWORD [rsp], " + reg);
	} else {
		appendOutput(instruction + " QWORD [rsp + " + std::to_string(offset) + "], " + reg);
	}
}

void Generator::push(const std::string &reg) {
	incrementStack();
	copyValueToStackFrom(reg);
}

void Generator::pop(const std::string &reg) {
	if (reg != "") copyValueFromStackTo(reg);
	decrementStack();
}

size_t Generator::stackSize = 0;
size_t Generator::labelCounter = 0;
std::vector<std::shared_ptr<Function>> Generator::functionTrace;
std::unique_ptr<std::vector<OutputLine>> Generator::output = std::make_unique<std::vector<OutputLine>>();

// need these lines otherwise will have linking error
std::vector<Variable> Generator::variables;
std::vector<size_t> Generator::numbersOfVariablesDeclaredBeforeScope;
std::vector<Data> Generator::data;
std::vector<std::shared_ptr<Function>> Generator::functions;
std::vector<std::shared_ptr<Function>> Generator::allFunctions;
std::vector<size_t> Generator::numbersOfFunctionsDeclaredBeforeScope;
