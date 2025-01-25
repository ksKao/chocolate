#pragma once
#include <memory>
#include <optional>
#include <string>

#include "AST/Scope.h"

struct OutputLine {
	std::string content;
	bool indent;
};

struct Variable {
	std::string name;
	size_t stackLocation;
};

struct Data {
	std::string size;  // DD, DW, DQ etc
	std::string value;
};

class Generator {
  public:
	static const int stackUnitSize = 16;

  public:
	static void appendOutput(const std::string &line, bool indent = true);
	static void appendComment(const std::string &line);

	static std::string getDataName(const std::string &value, const std::string &size = "DQ");
	static std::stringstream getOutput(const Scope &program);

	static void addVariable(const std::string &variableName);
	static std::optional<Variable> getVariable(const std::string variableName);

	static void startScope();
	static void endScope();

	static size_t getStackSize();
	static void incrementStack();
	static void decrementStack();

  private:
	// delete constructor for static class
	Generator() = delete;

	// size of the stack without counting the actual size of data in memory
	static size_t stackSize;

	// heap allocate as the output may get very big
	static std::unique_ptr<std::vector<OutputLine>> output;

	// stores all the variables in all scopes in the whole program
	static std::vector<Variable> variables;

	// each element stores the number of variables declared before the scope
	static std::vector<size_t> numbersOfVariablesDeclaredBeforeScope;

	// all data stored in the .data section
	static std::vector<Data> data;
};