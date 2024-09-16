#pragma once
#include "AST/Program.h"

#include <string>
#include <unordered_map>
#include <optional>
#include <memory>

struct OutputLine
{
    std::string content;
    bool indent;
};

struct Variable
{
    std::string name;
    size_t stackLocation;
};

struct Data
{
    std::string size; // DD, DW, DQ etc
    std::string value;
};

class Generator
{
public:
    static const int stackUnitSize = 8;

public:
    static void appendOutput(const std::string &line, bool indent = true);

    static std::string getDataName(const std::string &value, const std::string &size = "DD");
    static std::string getOutput(const Program &program);

    static void addVariable(const std::string &variableName);
    static std::optional<Variable> getVariable(const std::string variableName);

    static size_t getStackSize();
    static void incrementStack();
    static void decrementStack();

private:
    Generator() = delete;                                     // delete constructor for static class
    static size_t stackSize;                                  // size of the stack without counting the actual size of data in memory
    static std::unique_ptr<std::vector<OutputLine>> output;   // heap allocate as the output may get very big
    static std::unordered_map<std::string, size_t> variables; // key is the name of the variable, value is the location of this variable on the stack without counting the actual size in memory
    static std::vector<Data> data;                            // all data stored in the .data section
};