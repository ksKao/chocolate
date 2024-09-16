#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Token.h"

struct Node {
	virtual std::string getTypeName() const = 0;
	virtual void print(const std::string &indent) const = 0;
	virtual void generateAssembly() const = 0;
};