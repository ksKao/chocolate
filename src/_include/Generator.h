#pragma once
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "AST/Program.h"

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

	static std::string getDataName(const std::string &value, const std::string &size = "DQ");
	static std::string getOutput(const Program &program);

	static void addVariable(const std::string &variableName);
	static std::optional<Variable> getVariable(const std::string variableName);

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

	// key is the name of the variable, value is the location of this variable on the stack without counting the actual size in memory
	static std::unordered_map<std::string, size_t> variables;

	// all data stored in the .data section
	static std::vector<Data> data;
};