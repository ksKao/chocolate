#pragma once

#include "AST/Node.h"

struct Expression : Node {
	virtual std::string getTypeName() const = 0;
	virtual void print(const std::string &indent) const = 0;
	virtual void generateAssembly() const = 0;
};