#pragma once
#include "AST/Expression.h"

struct Identifier : Expression {
	std::string name;

	inline std::string getTypeName() const override {
		return "Identifier";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() const override;
};