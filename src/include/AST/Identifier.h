#pragma once
#include "AST/Node.h"

struct Identifier : Node {
	std::string name;

	inline std::string getTypeName() const override {
		return "Identifier";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() const override;
};